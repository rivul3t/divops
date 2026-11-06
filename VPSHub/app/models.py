from datetime import datetime, timezone
from sqlalchemy import Boolean, DateTime, ForeignKey, Integer, String, Text
from sqlalchemy.orm import DeclarativeBase, Mapped, mapped_column, relationship


def now():
    return datetime.now(timezone.utc)


class Base(DeclarativeBase):
    pass


class Tariff(Base):
    __tablename__ = "tariffs"
    id: Mapped[int] = mapped_column(primary_key=True)
    name: Mapped[str] = mapped_column(String(80), unique=True)
    cpu: Mapped[int] = mapped_column(Integer)
    ram: Mapped[int] = mapped_column(Integer)
    disk: Mapped[int] = mapped_column(Integer)
    price: Mapped[int] = mapped_column(Integer)
    description: Mapped[str] = mapped_column(Text)
    active: Mapped[bool] = mapped_column(Boolean, default=True)


class Order(Base):
    __tablename__ = "orders"
    id: Mapped[int] = mapped_column(primary_key=True)
    code: Mapped[str] = mapped_column(String(32), unique=True, index=True)
    tariff_id: Mapped[int] = mapped_column(ForeignKey("tariffs.id"))
    # Снимок условий: изменение тарифа не меняет уже созданную заявку.
    tariff_name: Mapped[str] = mapped_column(String(80))
    cpu: Mapped[int] = mapped_column(Integer)
    ram: Mapped[int] = mapped_column(Integer)
    disk: Mapped[int] = mapped_column(Integer)
    monthly_price: Mapped[int] = mapped_column(Integer)
    months: Mapped[int] = mapped_column(Integer)
    total: Mapped[int] = mapped_column(Integer)
    customer: Mapped[str] = mapped_column(String(100))
    email: Mapped[str] = mapped_column(String(254), index=True)
    os_name: Mapped[str] = mapped_column(String(80))
    comment: Mapped[str] = mapped_column(Text, default="")
    status: Mapped[str] = mapped_column(String(30), default="new")
    created_at: Mapped[datetime] = mapped_column(DateTime(timezone=True), default=now)
    events: Mapped[list["OrderEvent"]] = relationship(
        back_populates="order", cascade="all, delete-orphan", order_by="OrderEvent.id"
    )


class OrderEvent(Base):
    __tablename__ = "order_events"
    id: Mapped[int] = mapped_column(primary_key=True)
    order_id: Mapped[int] = mapped_column(ForeignKey("orders.id"))
    message: Mapped[str] = mapped_column(String(200))
    created_at: Mapped[datetime] = mapped_column(DateTime(timezone=True), default=now)
    order: Mapped[Order] = relationship(back_populates="events")
