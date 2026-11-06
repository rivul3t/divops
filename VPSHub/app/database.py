from pathlib import Path
from sqlalchemy import create_engine, event, select
from sqlalchemy.engine import make_url
from sqlalchemy.orm import sessionmaker
from .models import Base, Tariff


DEMO = [
    ("Start", 1, 1, 20, 300, "Для знакомства с Linux и небольших учебных проектов."),
    ("Web", 1, 2, 30, 500, "Для персонального сайта и простого веб-приложения."),
    ("Standard", 2, 4, 50, 800, "Для приложения с базой данных и командной работы."),
    ("Plus", 4, 6, 80, 1200, "Больше памяти для нескольких сервисов."),
    ("Pro", 4, 8, 100, 1600, "Для проектов с растущим потреблением ресурсов."),
    ("Team", 8, 16, 200, 3000, "Для нескольких приложений и тестовых сред команды."),
]


def build_database(url):
    parsed = make_url(url)
    sqlite = parsed.get_backend_name() == "sqlite"
    if sqlite and parsed.database and parsed.database != ":memory:":
        Path(parsed.database).parent.mkdir(parents=True, exist_ok=True)
    engine = create_engine(url, pool_pre_ping=True,
                           connect_args={"check_same_thread": False, "timeout": 15} if sqlite else {})
    if sqlite:
        @event.listens_for(engine, "connect")
        def configure_connection(connection, _):
            connection.execute("PRAGMA foreign_keys=ON")
    return engine, sessionmaker(engine, expire_on_commit=False)


def initialize(engine, session_factory, seed):
    Base.metadata.create_all(engine)
    # Один процесс приложения выполняет инициализацию; миграции — отдельное расширение курса.
    if seed:
        with session_factory.begin() as db:
            if db.scalar(select(Tariff.id).limit(1)) is None:
                db.add_all(Tariff(name=n, cpu=c, ram=r, disk=d, price=p, description=t)
                           for n, c, r, d, p, t in DEMO)
