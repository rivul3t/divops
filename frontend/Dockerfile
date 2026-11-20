FROM nginx:1.27-alpine

# Статика фронта.
COPY index.html /usr/share/nginx/html/index.html
COPY css /usr/share/nginx/html/css
COPY js  /usr/share/nginx/html/js

# Конфиг-шаблон: образ прогонит его через envsubst при старте и положит
# готовый результат в /etc/nginx/conf.d/default.conf.
COPY nginx.conf.template /etc/nginx/templates/default.conf.template

# Значения по умолчанию (переопределяются env в Deployment/compose).
ENV BACKEND_HOST=tsu-vps-service:8080 \
    DNS_RESOLVER=10.43.0.10

EXPOSE 80
