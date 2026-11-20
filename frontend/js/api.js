// Клиент REST API сервиса tsu_vps_service (см. tsu_vps_service/docs/API.md).
// Базовый адрес берётся из localStorage. По умолчанию пусто = тот же origin:
// запросы идут на /logic/... текущего хоста, где nginx проксирует их на бэкенд.
// Для локальной разработки без nginx впиши в шапке http://localhost:8080.

const DEFAULT_BASE = "";

const apiBase = {
  get() {
    return localStorage.getItem("apiBase") || DEFAULT_BASE;
  },
  set(url) {
    localStorage.setItem("apiBase", url.replace(/\/+$/, ""));
  },
};

class ApiError extends Error {
  constructor(message, status) {
    super(message);
    this.name = "ApiError";
    this.status = status;
  }
}

// Единая обёртка над fetch: сериализует JSON, разбирает ответ и ошибки.
async function request(method, path, body) {
  let res;
  try {
    res = await fetch(apiBase.get() + path, {
      method,
      headers: body ? { "Content-Type": "application/json" } : undefined,
      body: body ? JSON.stringify(body) : undefined,
    });
  } catch (e) {
    // Сетевая ошибка или блок CORS — fetch отклоняется без ответа.
    throw new ApiError("Нет связи с API (проверь адрес и CORS)", 0);
  }

  const text = await res.text();
  let data = null;
  if (text) {
    try {
      data = JSON.parse(text);
    } catch {
      data = text;
    }
  }

  if (!res.ok) {
    const msg = data && data.error ? data.error : `HTTP ${res.status}`;
    throw new ApiError(msg, res.status);
  }
  return data;
}

const api = {
  users: {
    list: () => request("GET", "/logic/user"),
    get: (id) => request("GET", `/logic/user/${id}`),
    create: (u) => request("POST", "/logic/user", u),
  },
  rates: {
    list: () => request("GET", "/logic/rate"),
    get: (id) => request("GET", `/logic/rate/${id}`),
    create: (r) => request("POST", "/logic/rate", r),
    update: (id, r) => request("PUT", `/logic/rate/${id}`, r),
    remove: (id) => request("DELETE", `/logic/rate/${id}`),
  },
  vms: {
    list: () => request("GET", "/logic/vm"),
    get: (id) => request("GET", `/logic/vm/${id}`),
    create: (v) => request("POST", "/logic/vm", v),
    remove: (id) => request("DELETE", `/logic/vm/${id}`),
  },
};
