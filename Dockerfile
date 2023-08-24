FROM debian:12.1-slim AS build-stage

RUN apt update
RUN apt install -y cmake wget

WORKDIR /node
RUN wget -O node.tar.gz https://nodejs.org/download/release/v16.20.2/node-v16.20.2-linux-x64.tar.gz
RUN tar -xaf node.tar.gz
RUN rm node.tar.gz
ENV PATH="${PATH}:/node/node-v16.20.2-linux-x64/bin"

WORKDIR /sdk
RUN wget -O wasisdk.tar.gz https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz
RUN tar -xaf wasisdk.tar.gz
RUN rm wasisdk.tar.gz

WORKDIR /app
COPY ./ ./

WORKDIR /app/src/core/wasm
RUN cmake . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/Platform/WASI.cmake -DWASI_SDK=/sdk/wasi-sdk-20.0
RUN cmake --build build
RUN cmake --install build

WORKDIR /app
RUN npm ci
RUN npm run build

FROM nginx:1.25.2-alpine
COPY --from=build-stage /app/dist /usr/share/nginx/html
