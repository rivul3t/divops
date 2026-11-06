const months = document.getElementById('months');
function updateTotal() {
  document.getElementById('total').textContent =
    new Intl.NumberFormat('ru-RU').format(Number(months.dataset.price) * Number(months.value)) + ' ₽';
}
if (months) { months.addEventListener('change', updateTotal); updateTotal(); }
document.querySelectorAll('form[data-confirm]').forEach(form => {
  form.addEventListener('submit', event => {
    if (!window.confirm(form.dataset.confirm)) event.preventDefault();
  });
});
