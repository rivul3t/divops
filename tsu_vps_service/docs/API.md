# API сервиса tsu_vps_service

REST API сервиса аренды VPS (ядро на C++, фреймворк [Drogon](https://github.com/drogonframework/drogon)).
Документация составлена по коду в `tsu_vps_service/core/service_logic/` (ветка `develop`).

## Общее

| Параметр | Значение |
|---|---|
| Базовый адрес | `http://<host>:8080` (слушает `0.0.0.0:8080`, см. `core_app/src/main.cpp`) |
| Формат тела | `application/json` |
| Префикс всех ручек | `/logic` |
| Аутентификация | **нет** (на текущем этапе ручки открыты) |

Ресурсы: **пользователи** (`/logic/user`), **тарифы** (`/logic/rate`),
**виртуальные машины** (`/logic/vm`).

### Коды ответов и ошибки

Успех зависит от метода: `200 OK` (чтение, успешные `PUT`/`DELETE`),
`201 Created` (создание), `404 Not Found` (объект не найден).

Ошибки приходят телом `{"error": "<текст>"}` (кроме `404`, который
возвращается с **пустым телом**). Код определяется типом исключения
(`helpers/src/exception.cpp`):

| Тип исключения | HTTP | Когда |
|---|---|---|
| `WrongArgument` | `400` | невалидный / пустой JSON, не распарсилось тело |
| `Forbidden` | `403` | (зарезервировано, пока не бросается) |
| `AlreadyExists` | `409` | (зарезервировано, пока не бросается) |
| `DatabaseError` | `500` | ошибка запроса к БД (в т.ч. нарушение `UNIQUE`) |
| `InternalError` | `500` | прочие ошибки |

---

## Модели данных

### User

Ответ на чтение пользователя:

```json
{
  "id": 1,
  "email": "user@example.com",
  "password": "secret",
  "role": "user",
  "created_at": "",
  "updated_at": ""
}
```

| Поле | Тип | Примечание |
|---|---|---|
| `id` | int | только в ответе |
| `email` | string | уникальный |
| `password` | string | см. раздел «Замечания» |
| `role` | string | `administrator` \| `auth_user` \| `user` |
| `created_at` | string | ISO-8601; сейчас всегда `""` (см. «Замечания») |
| `updated_at` | string | то же |

Тело на создание (`POST`) — без `id`, `created_at`, `updated_at` (ставятся сервером):

```json
{ "email": "user@example.com", "password": "secret", "role": "user" }
```

### Rate (тариф)

```json
{ "id": 1, "name": "basic", "ram_amount": 2048, "hd_amount": 50, "price": 300 }
```

| Поле | Тип | Примечание |
|---|---|---|
| `id` | int | только в ответе / в пути при `PUT` |
| `name` | string | уникальный |
| `ram_amount` | int | МБ |
| `hd_amount` | int | ГБ |
| `price` | number | в БД пишется как целое (дробная часть отбрасывается) |

Тело на создание/обновление — те же поля без `id`.

### VirtualMachine

```json
{ "id": 1, "user_id": 1, "rate_type": 1, "address": "10.0.0.5", "password": "secret" }
```

| Поле | Тип | Примечание |
|---|---|---|
| `id` | int | только в ответе |
| `user_id` | int | ссылка на пользователя |
| `rate_type` | int | id тарифа (в JSON поле называется `rate_type`, не `rate_id`) |
| `address` | string | адрес ВМ, уникальный |
| `password` | string | см. «Замечания» |

Тело на создание — те же поля без `id`.

---

## Эндпоинты

### Пользователи

#### `GET /logic/user`
Список всех пользователей.
- **200** — массив `User` (`[]`, если пусто).

```bash
curl http://localhost:8080/logic/user
```

#### `GET /logic/user/{id}`
Один пользователь по id.
- **200** — объект `User`.
- **404** — не найден (пустое тело).

```bash
curl http://localhost:8080/logic/user/1
```

#### `POST /logic/user`
Создать пользователя.
- **201** — создан; **в ответ возвращается присланный JSON**.
- **400** — тело пустое или невалидный JSON / неизвестная `role`.
- **500** — ошибка БД (например, `email` уже занят).

```bash
curl -X POST http://localhost:8080/logic/user \
  -H 'Content-Type: application/json' \
  -d '{"email":"user@example.com","password":"secret","role":"user"}'
```

---

### Тарифы

#### `GET /logic/rate`
Список тарифов.
- **200** — массив `Rate`.

#### `GET /logic/rate/{id}`
Тариф по id.
- **200** — `Rate`; **404** — не найден (пустое тело).

#### `POST /logic/rate`
Создать тариф.
- **201** — создан (**тело ответа пустое**).
- **400** — пустой/невалидный JSON; **500** — ошибка БД (`name` занят).

```bash
curl -X POST http://localhost:8080/logic/rate \
  -H 'Content-Type: application/json' \
  -d '{"name":"basic","ram_amount":2048,"hd_amount":50,"price":300}'
```

#### `PUT /logic/rate/{id}`
Обновить тариф целиком (id берётся из пути).
- **200** — обновлён; **404** — тарифа с таким id нет.
- **400** — тело не является JSON.

```bash
curl -X PUT http://localhost:8080/logic/rate/1 \
  -H 'Content-Type: application/json' \
  -d '{"name":"pro","ram_amount":4096,"hd_amount":100,"price":600}'
```

#### `DELETE /logic/rate/{id}`
Удалить тариф.
- **200** — удалён; **404** — не найден.

---

### Виртуальные машины

#### `GET /logic/vm`
Список ВМ.
- **200** — массив `VirtualMachine`.

#### `GET /logic/vm/{id}`
ВМ по id.
- **200** — `VirtualMachine`; **404** — не найдена (пустое тело).

#### `POST /logic/vm`
Создать ВМ.
- **201** — создана (**тело ответа пустое**).
- **400** — невалидный JSON; **500** — ошибка БД (несуществующий `user_id`/`rate_type`, занятый `address`).

```bash
curl -X POST http://localhost:8080/logic/vm \
  -H 'Content-Type: application/json' \
  -d '{"user_id":1,"rate_type":1,"address":"10.0.0.5","password":"secret"}'
```

#### `DELETE /logic/vm/{id}`
Удалить ВМ.
- **200** — удалена; **404** — не найдена.

---

## Сводная таблица

| Метод | Путь | Назначение | Успех |
|---|---|---|---|
| GET | `/logic/user` | список пользователей | 200 |
| GET | `/logic/user/{id}` | пользователь по id | 200 / 404 |
| POST | `/logic/user` | создать пользователя | 201 |
| GET | `/logic/rate` | список тарифов | 200 |
| GET | `/logic/rate/{id}` | тариф по id | 200 / 404 |
| POST | `/logic/rate` | создать тариф | 201 |
| PUT | `/logic/rate/{id}` | обновить тариф | 200 / 404 |
| DELETE | `/logic/rate/{id}` | удалить тариф | 200 / 404 |
| GET | `/logic/vm` | список ВМ | 200 |
| GET | `/logic/vm/{id}` | ВМ по id | 200 / 404 |
| POST | `/logic/vm` | создать ВМ | 201 |
| DELETE | `/logic/vm/{id}` | удалить ВМ | 200 / 404 |

> У пользователей нет `PUT`/`DELETE`, у ВМ нет `PUT` — ручки просто не реализованы.

---