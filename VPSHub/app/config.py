import os
from dataclasses import dataclass
from dotenv import load_dotenv


@dataclass
class Settings:
    database_url: str
    session_secret: str
    admin_username: str
    admin_password: str
    cookie_secure: bool = False
    seed_demo: bool = True
    app_name: str = "VPSHub"

    @classmethod
    def from_env(cls):
        load_dotenv()
        secret = os.getenv("SESSION_SECRET", "")
        password = os.getenv("ADMIN_PASSWORD", "")
        if len(secret) < 32 or secret.startswith("REPLACE_"):
            raise RuntimeError("Укажите случайный SESSION_SECRET (от 32 символов) в .env")
        if len(password) < 12 or password.startswith("REPLACE_"):
            raise RuntimeError("Укажите ADMIN_PASSWORD (от 12 символов) в .env")
        return cls(
            database_url=os.getenv("DATABASE_URL", "sqlite:///./data/vpshub.db"),
            session_secret=secret,
            admin_username=os.getenv("ADMIN_USERNAME", "manager"),
            admin_password=password,
            cookie_secure=os.getenv("COOKIE_SECURE", "false").lower() == "true",
            seed_demo=os.getenv("SEED_DEMO", "true").lower() == "true",
            app_name=os.getenv("APP_NAME", "VPSHub"),
        )
