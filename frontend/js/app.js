// UI-логика панели: вкладки, формы, таблицы. Зависит от api.js (глобальные api, apiBase).

// Кэши для подстановки имён в таблице ВМ (user_id -> email, rate_type -> name).
let usersCache = [];
let ratesCache = [];

// ---------- утилиты ----------

const $ = (sel, root = document) => root.querySelector(sel);
const $$ = (sel, root = document) => [...root.querySelectorAll(sel)];

function toast(message, type = "ok") {
  const el = $("#toast");
  el.textContent = message;
  el.className = `toast show ${type}`;
  clearTimeout(toast._t);
  toast._t = setTimeout(() => (el.className = "toast"), 3500);
}

function esc(value) {
  return String(value ?? "").replace(/[&<>"']/g, (c) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;",
  }[c]));
}

// Собирает объект из полей формы по заданной схеме типов.
function readForm(form, schema) {
  const data = {};
  for (const [name, type] of Object.entries(schema)) {
    const raw = form.elements[name]?.value ?? "";
    data[name] = type === "number" ? Number(raw) : raw;
  }
  return data;
}

// ---------- вкладки ----------

function initTabs() {
  $$(".tab").forEach((tab) => {
    tab.addEventListener("click", () => {
      $$(".tab").forEach((t) => t.classList.remove("is-active"));
      $$(".tab-panel").forEach((p) => p.classList.remove("is-active"));
      tab.classList.add("is-active");
      $(`#tab-${tab.dataset.tab}`).classList.add("is-active");
    });
  });

  $$("[data-refresh]").forEach((btn) => {
    btn.addEventListener("click", () => loaders[btn.dataset.refresh]());
  });
}

// ---------- адрес API ----------

function initApiBase() {
  const input = $("#api-base");
  input.value = apiBase.get();
  $("#api-status").textContent = "";

  $("#api-save").addEventListener("click", () => {
    // Пустое поле = тот же origin (запросы на /logic через nginx).
    apiBase.set(input.value.trim());
    input.value = apiBase.get();
    toast("Адрес API сохранён");
    loadAll();
  });
}

// ---------- пользователи ----------

async function loadUsers() {
  const tbody = $("#users-rows");
  try {
    const users = await api.users.list();
    usersCache = users || [];
    tbody.innerHTML = usersCache.length
      ? usersCache.map((u) => `
        <tr>
          <td>${esc(u.id)}</td>
          <td>${esc(u.email)}</td>
          <td><span class="badge">${esc(u.role)}</span></td>
          <td class="muted">${esc(u.created_at) || "—"}</td>
        </tr>`).join("")
      : `<tr><td colspan="4" class="muted">Пусто</td></tr>`;
  } catch (e) {
    tbody.innerHTML = `<tr><td colspan="4" class="error">Ошибка: ${esc(e.message)}</td></tr>`;
  }
}

function initUserForm() {
  $("#user-form").addEventListener("submit", async (ev) => {
    ev.preventDefault();
    const payload = readForm(ev.target, { email: "string", password: "string", role: "string" });
    try {
      await api.users.create(payload);
      toast("Пользователь создан");
      ev.target.reset();
      await loadUsers();
      fillVmSelectors();
    } catch (e) {
      toast(e.message, "error");
    }
  });
}

// ---------- тарифы ----------

async function loadRates() {
  const tbody = $("#rates-rows");
  try {
    const rates = await api.rates.list();
    ratesCache = rates || [];
    tbody.innerHTML = ratesCache.length
      ? ratesCache.map((r) => `
        <tr>
          <td>${esc(r.id)}</td>
          <td>${esc(r.name)}</td>
          <td>${esc(r.ram_amount)} МБ</td>
          <td>${esc(r.hd_amount)} ГБ</td>
          <td>${esc(r.price)}</td>
          <td class="row-actions">
            <button class="btn btn-sm" data-edit-rate="${esc(r.id)}">✎</button>
            <button class="btn btn-sm btn-danger" data-del-rate="${esc(r.id)}">✕</button>
          </td>
        </tr>`).join("")
      : `<tr><td colspan="6" class="muted">Пусто</td></tr>`;

    $$("[data-del-rate]", tbody).forEach((b) =>
      b.addEventListener("click", () => deleteRate(b.dataset.delRate)));
    $$("[data-edit-rate]", tbody).forEach((b) =>
      b.addEventListener("click", () => startEditRate(b.dataset.editRate)));
  } catch (e) {
    tbody.innerHTML = `<tr><td colspan="6" class="error">Ошибка: ${esc(e.message)}</td></tr>`;
  }
}

function startEditRate(id) {
  const rate = ratesCache.find((r) => String(r.id) === String(id));
  if (!rate) return;
  const form = $("#rate-form");
  form.elements.id.value = rate.id;
  form.elements.name.value = rate.name;
  form.elements.ram_amount.value = rate.ram_amount;
  form.elements.hd_amount.value = rate.hd_amount;
  form.elements.price.value = rate.price;
  $("#rate-form-title").textContent = `Редактирование тарифа #${rate.id}`;
  $("#rate-submit").textContent = "Сохранить";
  $("#rate-cancel").classList.remove("hidden");
  form.scrollIntoView({ behavior: "smooth", block: "center" });
}

function resetRateForm() {
  const form = $("#rate-form");
  form.reset();
  form.elements.id.value = "";
  $("#rate-form-title").textContent = "Новый тариф";
  $("#rate-submit").textContent = "Создать";
  $("#rate-cancel").classList.add("hidden");
}

async function deleteRate(id) {
  if (!confirm(`Удалить тариф #${id}?`)) return;
  try {
    await api.rates.remove(id);
    toast("Тариф удалён");
    await loadRates();
    fillVmSelectors();
  } catch (e) {
    toast(e.message, "error");
  }
}

function initRateForm() {
  $("#rate-cancel").addEventListener("click", resetRateForm);

  $("#rate-form").addEventListener("submit", async (ev) => {
    ev.preventDefault();
    const form = ev.target;
    const id = form.elements.id.value;
    const payload = readForm(form, {
      name: "string", ram_amount: "number", hd_amount: "number", price: "number",
    });
    try {
      if (id) {
        await api.rates.update(id, payload);
        toast("Тариф обновлён");
      } else {
        await api.rates.create(payload);
        toast("Тариф создан");
      }
      resetRateForm();
      await loadRates();
      fillVmSelectors();
    } catch (e) {
      toast(e.message, "error");
    }
  });
}

// ---------- виртуалки ----------

// Заполняет выпадашки пользователей и тарифов в форме ВМ из кэшей.
function fillVmSelectors() {
  const userSel = $("#vm-user");
  const rateSel = $("#vm-rate");
  userSel.innerHTML = usersCache.length
    ? usersCache.map((u) => `<option value="${esc(u.id)}">#${esc(u.id)} — ${esc(u.email)}</option>`).join("")
    : `<option value="" disabled>нет пользователей</option>`;
  rateSel.innerHTML = ratesCache.length
    ? ratesCache.map((r) => `<option value="${esc(r.id)}">#${esc(r.id)} — ${esc(r.name)}</option>`).join("")
    : `<option value="" disabled>нет тарифов</option>`;
}

async function loadVms() {
  const tbody = $("#vms-rows");
  try {
    const vms = await api.vms.list();
    tbody.innerHTML = (vms && vms.length)
      ? vms.map((v) => {
          const user = usersCache.find((u) => String(u.id) === String(v.user_id));
          const rate = ratesCache.find((r) => String(r.id) === String(v.rate_type));
          return `
            <tr>
              <td>${esc(v.id)}</td>
              <td>${user ? esc(user.email) : "#" + esc(v.user_id)}</td>
              <td>${rate ? esc(rate.name) : "#" + esc(v.rate_type)}</td>
              <td>${esc(v.address)}</td>
              <td class="row-actions">
                <button class="btn btn-sm btn-danger" data-del-vm="${esc(v.id)}">✕</button>
              </td>
            </tr>`;
        }).join("")
      : `<tr><td colspan="5" class="muted">Пусто</td></tr>`;

    $$("[data-del-vm]", tbody).forEach((b) =>
      b.addEventListener("click", () => deleteVm(b.dataset.delVm)));
  } catch (e) {
    tbody.innerHTML = `<tr><td colspan="5" class="error">Ошибка: ${esc(e.message)}</td></tr>`;
  }
}

async function deleteVm(id) {
  if (!confirm(`Удалить виртуалку #${id}?`)) return;
  try {
    await api.vms.remove(id);
    toast("Виртуалка удалена");
    await loadVms();
  } catch (e) {
    toast(e.message, "error");
  }
}

function initVmForm() {
  $("#vm-form").addEventListener("submit", async (ev) => {
    ev.preventDefault();
    const payload = readForm(ev.target, {
      user_id: "number", rate_type: "number", address: "string", password: "string",
    });
    try {
      await api.vms.create(payload);
      toast("Виртуалка создана");
      ev.target.reset();
      await loadVms();
    } catch (e) {
      toast(e.message, "error");
    }
  });
}

// ---------- загрузка ----------

const loaders = { users: loadUsers, rates: loadRates, vms: loadVms };

// Грузим пользователей и тарифы первыми — они нужны для селекторов и имён в ВМ.
async function loadAll() {
  await Promise.all([loadUsers(), loadRates()]);
  fillVmSelectors();
  await loadVms();
}

function init() {
  initTabs();
  initApiBase();
  initUserForm();
  initRateForm();
  initVmForm();
  loadAll();
}

document.addEventListener("DOMContentLoaded", init);
