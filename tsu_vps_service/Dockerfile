FROM drogonframework/drogon:latest

RUN apt-get update \
 && apt-get install -y --no-install-recommends libspdlog-dev \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . ./tsu_vps_service

RUN cmake -S tsu_vps_service -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build -j"$(nproc)"

EXPOSE 8080
CMD ["/src/build/core/core_app/core_app"]
