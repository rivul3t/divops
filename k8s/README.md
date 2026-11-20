# Деплой приложения в k3s

Стек: PostgreSQL + бэкенд (`tsu_vps_service`) + фронт (nginx со статикой, проксит
`/logic` на бэкенд). Всё в namespace `vpshub`.

```
фронт (nginx) ──/logic──> backend:8080 ──> postgres:5432
     :80                   (Drogon)          (PVC + init из tables.sql)
Ingress (traefik) ──> frontend
```

## Доставка образов

k3s использует **containerd, а не docker** — образы из докера не видит. Поэтому образ надо собрать и импортировать в containerd ноды:

```bash
docker build -t tsu-vps-backend:0.1.0 tsu_vps_service
docker build -t vpshub-frontend:0.1.0 frontend

# залить в k3s на ноде
docker save tsu-vps-backend:0.1.0 | ssh <node> 'sudo k3s ctr images import -'
docker save vpshub-frontend:0.1.0 | ssh <node> 'sudo k3s ctr images import -'
```

Манифесты используют тег `0.1.0` и `imagePullPolicy: IfNotPresent` — k8s берёт
импортированный образ и не лезет во внешний реестр. Проверить, что образ на месте:
`sudo k3s ctr images ls | grep 0.1.0`.


## Деплой

`tables.sql` лежит вне `k8s/`, поэтому kustomize запускается с флагом
`--load-restrictor` (иначе ругается на путь наружу):

```bash
kubectl kustomize --load-restrictor LoadRestrictionsNone k8s | kubectl apply -f -
kubectl -n vpshub get pods,svc,ingress
```

Порядок подъёма разруливается сам: postgres прогоняет `tables.sql` при первом
старте (пустой том), backend ждёт порт postgres через init-контейнер.

Открыть панель — по IP ноды:
```bash
curl http://<node-ip>/
```

## Обновление

Лучший путь — бампнуть версию тега (`0.1.1`, ...), собрать, импортировать, обновить
`image:` в манифесте и `kubectl apply` — k8s сам подхватит новый образ и пересоздаст под.

Если переиспользуешь тот же тег, образ для k8s «не меняется» — нужен ручной рестарт:
```bash
kubectl -n vpshub rollout restart deployment/backend   # или frontend
```

## Снос

```bash
kubectl delete namespace vpshub 
```
Том `postgres-data` уходит вместе с namespace — данные БД пропадут.

## Секреты и БД

`postgres-secret.yaml` задаёт `POSTGRES_DB/USER/PASSWORD`. Бэкенд читает те же
значения из этого секрета (`DB_NAME/DB_USER/DB_PASSWORD`) + `DB_HOST=postgres`.
Поменять креды — только в секрете, бэкенд подхватит.
