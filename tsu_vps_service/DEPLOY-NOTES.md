# Правки под деплой (k8s/Docker)

Что меняли в коде/схеме, чтобы сервис собрался в контейнере и поднялся в k3s.
Бизнес-логику не трогали. Если что-то из этого мешает — обсуждаемо.

## Код

### `core/core_app/src/main.cpp` — коннект к БД из окружения
Было: хост/порт/имя/юзер/пароль захардкожены в `createDbClient`.
Стало: читаются из env `DB_HOST / DB_PORT / DB_NAME / DB_USER / DB_PASSWORD`,
дефолты = прежние значения. Без env локальный запуск работает как раньше; в
кубере прилетает `DB_HOST=postgres` и креды из секрета.

### `core/service_logic/logic/src/user_handler.cpp` — `SetUser`, INSERT
Было: `VALUES ($1, $2, $3, $4, $5)`, где `$4/$5` = `ToEpochMs()` (bigint, мс).
Стало: `... to_timestamp($4::bigint / 1000.0), to_timestamp($5::bigint / 1000.0)`.
Почему: колонки `created_at/updated_at` — `TIMESTAMPTZ`, вставка bigint в них
падала, POST `/logic/user` отдавал 500. Чтение (`EXTRACT(EPOCH ...)*1000`) не
трогали, round-trip сходится. Тип колонок не меняли.
Каст `::bigint` обязателен: без него из-за `/1000.0` postgres выводит тип `$4`
как `numeric`, а Drogon шлёт бинарный int8 → `invalid sign in external "numeric"
value`. Явный каст выравнивает тип параметра.

### `core/service_logic/logic/user_handler.h` — инклюд
Добавили `#include <optional>` (хедер объявляет `std::optional<User>`, инклюда
не было). Локально собиралось на транзитивных инклюдах, в образе — нет. У
`rate_handler.h`/`vm_handler.h` этот инклюд уже был.

## Схема

### `sql/tables.sql` — таблица ВМ
`vm_data` → `virtual_machines`, колонка `service_rate` → `rate_id`.
Почему: код в `logic/vm_handler.cpp` уже ходит в `virtual_machines` с `rate_id`
(`SELECT/INSERT`), старое имя таблицы/колонки коду не соответствовало — любой
запрос `/logic/vm` падал в 500.

## CMake (сборка)

### `CMakeLists.txt` (корень) + `core/CMakeLists.txt` + `core/helpers/CMakeLists.txt`
Убрали захардкоженный путь `list(APPEND CMAKE_PREFIX_PATH "/home/nobahu/libs/drogon")`.
Вместо него в корневом CMakeLists — опциональный `DROGON_ROOT`:
```cmake
if(DEFINED ENV{DROGON_ROOT})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{DROGON_ROOT}")
endif()
```
Локально: `export DROGON_ROOT=/home/nobahu/libs/drogon` (или где у тебя Drogon).
В контейнере Drogon системный, переменная не нужна.

### `core/service_logic/CMakeLists.txt` — убрали дубль
Удалили повторный `find_package(Drogon REQUIRED)` (он уже есть в
`core/CMakeLists.txt`). Из-за второго вызова в окружении с MySQL падала
конфигурация: `add_library cannot create imported target "MySQL_lib" ... already
exists`. `Drogon::Drogon` виден в подкаталоге из родительского `find_package`,
линковка не пострадала.

## Что НЕ трогали (на твоё усмотрение)

- `from_iso8601()` — заглушка, возвращает `""`; `created_at/updated_at` в JSON
  пустые (хранение теперь работает, просто вывод пустой).
- GET отдаёт «мусорный» `password` — пути записи/чтения по-разному трактуют
  base64.
- `price` кастится в `int` — теряется дробь.
- дубликат `email/name/address` → 500 вместо 409 (`AlreadyExists` нигде не
  бросается).
- `GetRate` — два одинаковых `catch` подряд (мёртвый код, компилится с варнингом
  `-Wexceptions`).
