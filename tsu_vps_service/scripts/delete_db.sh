#!/bin/bash
set -e

DB_NAME="service_db"
DB_PASSWRD="11111111"
DB_USER="postgres"
DB_HOST="localhost"
DB_PORT="5432"

# Передаём пароль psql через переменную окружения
export PGPASSWORD="$DB_PASSWRD"

echo "Удаление БД $DB_NAME"
psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d postgres \
     -c "DROP DATABASE IF EXISTS $DB_NAME WITH (FORCE);"

unset PGPASSWORD

echo "БД $DB_NAME удалена"