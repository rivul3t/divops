CREATE TABLE IF NOT EXISTS user_data (
    id              SERIAL PRIMARY KEY,
    email           VARCHAR NOT NULL UNIQUE,
    password        VARCHAR NOT NULL,
    role            INTEGER NOT NULL,
    created_at      TIMESTAMPTZ NOT NULL,
    updated_at      TIMESTAMPTZ NOT NULL
);

CREATE TABLE IF NOT EXISTS vm_data (
    id              SERIAL PRIMARY KEY,
    user_id         INTEGER NOT NULL REFERENCES user_data(id) ON DELETE CASCADE,
    address         VARCHAR NOT NULL UNIQUE,
    password        VARCHAR NOT NULL,
    rate_type       VARCHAR NOT NULL
);

CREATE TABLE IF NOT EXISTS service_rates (
    id              SERIAL PRIMARY KEY,
    name            VARCHAR NOT NULL UNIQUE,
    ram_amount      INTEGER NOT NULL,
    hd_amount       INTEGER NOT NULL,
    price           INTEGER NOT NULL
);
