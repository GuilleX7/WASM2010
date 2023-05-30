# Required dependencies:
# make & wasi-sdk for asm2010
# node & npm for the web application
FROM ubuntu:22.04 AS build-stage
RUN apt update && \
apt install -y wget make libxml2
WORKDIR /node
RUN wget -O node.tar.gz https://nodejs.org/download/release/v16.18.0/node-v16.18.0-linux-x64.tar.gz
RUN tar -xaf node.tar.gz
RUN rm node.tar.gz
ENV PATH="${PATH}:/node/node-v16.18.0-linux-x64/bin"
WORKDIR /sdk
RUN wget -O wasisdk.tar.gz https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz
RUN tar -xaf wasisdk.tar.gz
RUN rm wasisdk.tar.gz
WORKDIR /app
COPY ./ ./
WORKDIR /app/src/asm2010
RUN make WASI_SDK="/sdk/wasi-sdk-20.0" all
WORKDIR /app
RUN npm ci
RUN npm run build

FROM nginx:1.25-alpine
COPY --from=build-stage /app/dist /usr/share/nginx/html
