import logging
import re
import secrets
from contextlib import asynccontextmanager
from pathlib import Path
from typing import Annotated

from fastapi import Depends, FastAPI, HTTPException, Request
from fastapi.responses import HTMLResponse, JSONResponse, RedirectResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from sqlalchemy import func, or_, select, text, update
from sqlalchemy.exc import IntegrityError, SQLAlchemyError
from sqlalchemy.orm import Session
from starlette.middleware.sessions import SessionMiddleware

from .config import Settings
from .database import build_database, initialize
from .models import Order, OrderEvent, Tariff

logger = logging.getLogger("uvicorn.error")
ROOT = Path(__file__).parent
STATUSES = {"new": "Новая", "processing": "В обработке", "completed": "Выполнена",
            "rejected": "Отклонена", "cancelled": "Отменена клиентом"}
TRANSITIONS = {"new": ("processing", "rejected"), "processing": ("completed", "rejected"),
               "completed": (), "rejected": (), "cancelled": ()}
OS_OPTIONS = ("Ubuntu", "Debian", "AlmaLinux")
MONTHS = (1, 3, 6, 12)


def create_app(settings: Settings | None = None):
    settings = settings or Settings.from_env()
    engine, sessions = build_database(settings.database_url)

    @asynccontextmanager
    async def lifespan(app):
        initialize(engine, sessions, settings.seed_demo)
        yield
        engine.dispose()

    app = FastAPI(title=settings.app_name, lifespan=lifespan)
    app.state.engine = engine
    app.state.sessions = sessions
    app.add_middleware(SessionMiddleware, secret_key=settings.session_secret,
                       session_cookie="vpshub_session", same_site="lax",
                       https_only=settings.cookie_secure, max_age=8 * 3600)
    app.mount("/static", StaticFiles(directory=ROOT / "static"), name="static")
    templates = Jinja2Templates(directory=ROOT / "templates")
    templates.env.filters["money"] = lambda value: f"{int(value):,}".replace(",", " ") + " ₽"

    def render(request, name, *, status_code=200, **context):
        token = request.session.setdefault("csrf", secrets.token_urlsafe(32))
        return templates.TemplateResponse(request=request, name=name,
            context={"app_name": settings.app_name, "csrf": token,
                     "is_admin": request.session.get("admin", False), "statuses": STATUSES,
                     "transitions": TRANSITIONS, "os_options": OS_OPTIONS, "months_options": MONTHS,
                     **context}, status_code=status_code)

    def get_db():
        with sessions() as db:
            yield db

    DB = Annotated[Session, Depends(get_db)]

    async def form_data(request):
        data = await request.form(max_fields=30, max_files=0)
        supplied = str(data.get("csrf", ""))
        expected = request.session.get("csrf", "")
        if not expected or not secrets.compare_digest(supplied, expected):
            raise HTTPException(403, "Форма устарела. Обновите страницу и повторите действие.")
        return {k: str(v) if k == "password" else str(v).strip() for k, v in data.items()}

    def require_admin(request: Request):
        if not request.session.get("admin"):
            raise HTTPException(401, "Войдите как менеджер через /admin/login")

    def get_order(db, code):
        order = db.scalar(select(Order).where(Order.code == code))
        if order is None:
            raise HTTPException(404, "Заявка не найдена")
        return order

    def authorize(request, order):
        if not request.session.get("admin") and order.code not in request.session.get("orders", []):
            raise HTTPException(404, "Заявка не найдена. Используйте поиск по email и коду доступа.")

    def remember(request, code):
        codes = request.session.get("orders", [])
        request.session["orders"] = ([c for c in codes if c != code] + [code])[-20:]

    def change_status(db, order, target, actor):
        allowed = (order.status in ("new", "processing") if target == "cancelled"
                   else target in TRANSITIONS[order.status])
        if not allowed:
            raise HTTPException(409, "Этот переход статуса недоступен. Обновите страницу.")
        result = db.execute(update(Order).where(Order.id == order.id, Order.status == order.status)
                            .values(status=target).execution_options(synchronize_session=False))
        if result.rowcount != 1:
            db.rollback()
            raise HTTPException(409, "Заявка уже изменена. Обновите страницу.")
        db.add(OrderEvent(order_id=order.id, message=f"{actor}: {STATUSES[target]}"))
        db.commit()

    @app.exception_handler(HTTPException)
    async def http_error(request, exc):
        return render(request, "error.html", status_code=exc.status_code,
                      error=exc.detail, code=exc.status_code)

    @app.exception_handler(SQLAlchemyError)
    async def database_error(request, exc):
        logger.error("Database operation failed (%s)", type(exc).__name__)
        return render(request, "error.html", status_code=503, code=503,
                      error="База данных недоступна. Повторите запрос позже.")

    @app.get("/health")
    def health():
        return {"status": "ok", "service": "vpshub"}

    @app.get("/ready")
    def ready():
        try:
            with engine.connect() as connection:
                connection.execute(text("SELECT 1"))
            return {"status": "ready"}
        except SQLAlchemyError:
            return JSONResponse({"status": "not_ready"}, status_code=503)

    @app.get("/", response_class=HTMLResponse)
    def catalog(request: Request, db: DB):
        q = request.query_params.get("q", "")[:100].strip()
        def number(name):
            try:
                return max(0, min(int(request.query_params.get(name, "0")), 1_000_000))
            except ValueError:
                return 0
        cpu, ram, disk, budget = (number(k) for k in ("cpu", "ram", "disk", "budget"))
        query = select(Tariff).where(Tariff.active.is_(True))
        if q:
            query = query.where(or_(Tariff.name.ilike(f"%{q}%"), Tariff.description.ilike(f"%{q}%")))
        query = query.where(Tariff.cpu >= cpu, Tariff.ram >= ram, Tariff.disk >= disk)
        if budget:
            query = query.where(Tariff.price <= budget)
        return render(request, "catalog.html", tariffs=db.scalars(query.order_by(Tariff.price)).all(),
                      filters=dict(q=q, cpu=cpu, ram=ram, disk=disk, budget=budget))

    @app.get("/order/new/{tariff_id}")
    def new_order(tariff_id: int, request: Request, db: DB):
        tariff = db.get(Tariff, tariff_id)
        if not tariff or not tariff.active:
            raise HTTPException(404, "Тариф недоступен для новых заявок")
        return render(request, "order_form.html", tariff=tariff, values={}, error=None)

    @app.post("/order/new/{tariff_id}")
    async def submit_order(tariff_id: int, request: Request, db: DB):
        data = await form_data(request)
        tariff = db.get(Tariff, tariff_id)
        if not tariff or not tariff.active:
            raise HTTPException(404, "Тариф недоступен для новых заявок")
        error = None
        if not 2 <= len(data.get("customer", "")) <= 100:
            error = "Имя должно содержать от 2 до 100 символов."
        elif len(data.get("email", "")) > 254 or not re.fullmatch(r"[^\s@]+@[^\s@]+\.[^\s@]+", data.get("email", "")):
            error = "Укажите корректный email."
        elif data.get("months") not in {str(m) for m in MONTHS}:
            error = "Выберите срок из списка."
        elif data.get("os_name") not in OS_OPTIONS:
            error = "Выберите операционную систему из списка."
        elif len(data.get("comment", "")) > 1000:
            error = "Комментарий не должен превышать 1000 символов."
        if error:
            return render(request, "order_form.html", tariff=tariff, values=data,
                          error=error, status_code=400)
        months = int(data["months"])
        order = Order(code=secrets.token_hex(12), tariff_id=tariff.id,
                      tariff_name=tariff.name, cpu=tariff.cpu, ram=tariff.ram, disk=tariff.disk,
                      monthly_price=tariff.price, months=months, total=tariff.price * months,
                      customer=data["customer"], email=data["email"].lower(),
                      os_name=data["os_name"], comment=data.get("comment", ""))
        order.events.append(OrderEvent(message="Клиент: заявка создана"))
        db.add(order)
        db.commit()
        remember(request, order.code)
        return RedirectResponse(f"/orders/{order.code}", status_code=303)

    @app.get("/orders")
    def my_orders(request: Request, db: DB):
        orders = db.scalars(select(Order).where(Order.code.in_(request.session.get("orders", [])))
                            .order_by(Order.id.desc())).all()
        return render(request, "orders.html", orders=orders, error=None)

    @app.post("/orders/find")
    async def find_order(request: Request, db: DB):
        data = await form_data(request)
        order = db.scalar(select(Order).where(Order.code == data.get("code", "").lower(),
                                              Order.email == data.get("email", "").lower()))
        if not order:
            return render(request, "orders.html", orders=[], error="Заявка с таким email и кодом не найдена.",
                          status_code=404)
        remember(request, order.code)
        return RedirectResponse(f"/orders/{order.code}", status_code=303)

    @app.get("/orders/{code}")
    def order_detail(code: str, request: Request, db: DB):
        order = get_order(db, code)
        authorize(request, order)
        return render(request, "order_detail.html", order=order)

    @app.post("/orders/{code}/cancel")
    async def cancel_order(code: str, request: Request, db: DB):
        await form_data(request)
        order = get_order(db, code)
        authorize(request, order)
        change_status(db, order, "cancelled", "Клиент")
        return RedirectResponse(f"/orders/{code}", status_code=303)

    @app.get("/admin/login")
    def login_page(request: Request):
        return render(request, "login.html", error=None)

    @app.post("/admin/login")
    async def login(request: Request):
        data = await form_data(request)
        user_ok = secrets.compare_digest(data.get("username", "").encode(), settings.admin_username.encode())
        pass_ok = secrets.compare_digest(data.get("password", "").encode(), settings.admin_password.encode())
        if not (user_ok and pass_ok):
            return render(request, "login.html", error="Неверный логин или пароль.", status_code=401)
        codes = request.session.get("orders", [])
        request.session.clear()
        request.session.update(admin=True, orders=codes)
        return RedirectResponse("/admin", status_code=303)

    @app.post("/admin/logout")
    async def logout(request: Request):
        await form_data(request)
        request.session.clear()
        return RedirectResponse("/", status_code=303)

    @app.get("/admin", dependencies=[Depends(require_admin)])
    def admin(request: Request, db: DB):
        status = request.query_params.get("status", "")
        q = request.query_params.get("q", "")[:100].strip()
        query = select(Order)
        if status in STATUSES:
            query = query.where(Order.status == status)
        if q:
            query = query.where(or_(Order.email.ilike(f"%{q}%"), Order.customer.ilike(f"%{q}%"),
                                    Order.code.ilike(f"%{q}%")))
        stats = dict(db.execute(select(Order.status, func.count()).group_by(Order.status)).all())
        return render(request, "admin.html", orders=db.scalars(query.order_by(Order.id.desc()).limit(200)).all(),
                      tariffs=db.scalars(select(Tariff).order_by(Tariff.price)).all(),
                      stats=stats, status=status, q=q)

    @app.post("/admin/orders/{code}/status", dependencies=[Depends(require_admin)])
    async def update_status(code: str, request: Request, db: DB):
        data = await form_data(request)
        target = data.get("status", "")
        if target not in ("processing", "completed", "rejected"):
            raise HTTPException(400, "Недопустимый статус")
        change_status(db, get_order(db, code), target, "Менеджер")
        return RedirectResponse(f"/orders/{code}", status_code=303)

    @app.get("/admin/tariffs/new", dependencies=[Depends(require_admin)])
    def new_tariff(request: Request):
        return render(request, "tariff_form.html", tariff=None, values={}, error=None)

    @app.get("/admin/tariffs/{tariff_id}/edit", dependencies=[Depends(require_admin)])
    def edit_tariff(tariff_id: int, request: Request, db: DB):
        tariff = db.get(Tariff, tariff_id)
        if not tariff:
            raise HTTPException(404, "Тариф не найден")
        return render(request, "tariff_form.html", tariff=tariff, values={}, error=None)

    async def save_tariff(request, db, tariff=None):
        data = await form_data(request)
        error = None
        numbers = {}
        if not 2 <= len(data.get("name", "")) <= 80:
            error = "Название должно содержать от 2 до 80 символов."
        elif not 5 <= len(data.get("description", "")) <= 500:
            error = "Описание должно содержать от 5 до 500 символов."
        for field, maximum in (("cpu", 128), ("ram", 1024), ("disk", 100_000), ("price", 1_000_000)):
            try:
                value = int(data.get(field, ""))
                if not 1 <= value <= maximum:
                    raise ValueError
                numbers[field] = value
            except ValueError:
                error = f"Поле {field}: целое число от 1 до {maximum}."
        if error:
            return render(request, "tariff_form.html", tariff=tariff, values=data, error=error, status_code=400)
        obj = tariff or Tariff()
        obj.name = data["name"]
        obj.description = data["description"]
        obj.active = data.get("active") == "on"
        for key, value in numbers.items():
            setattr(obj, key, value)
        db.add(obj)
        try:
            db.commit()
        except IntegrityError:
            db.rollback()
            return render(request, "tariff_form.html", tariff=tariff, values=data,
                          error="Название тарифа уже используется.", status_code=400)
        return RedirectResponse("/admin#tariffs", status_code=303)

    @app.post("/admin/tariffs/new", dependencies=[Depends(require_admin)])
    async def create_tariff(request: Request, db: DB):
        return await save_tariff(request, db)

    @app.post("/admin/tariffs/{tariff_id}/edit", dependencies=[Depends(require_admin)])
    async def update_tariff(tariff_id: int, request: Request, db: DB):
        tariff = db.get(Tariff, tariff_id)
        if not tariff:
            raise HTTPException(404, "Тариф не найден")
        return await save_tariff(request, db, tariff)

    return app
