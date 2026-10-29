#!/bin/bash
set -e

DB_NAME="service_db"
DB_PASSWRD="11111111"
DB_USER="postgres"
DB_HOST="localhost"
DB_PORT="5432"

export PGPASSWORD="$DB_PASSWRD"

echo "Создание БД $DB_NAME"
psql -h $DB_HOST -p $DB_PORT -U $DB_USER -d postgres -c "CREATE DATABASE $DB_NAME;" 2>/dev/null

echo "📦 Выполняем SQL файлы из $SQL_DIR..."

psql -h $DB_HOST -p $DB_PORT -U $DB_USER -d $DB_NAME -f "../sql/tables.sql"

unset PGPASSWORD

echo "БД создана"