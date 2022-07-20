FROM node:16-alpine AS build
WORKDIR /app
COPY ./ ./
RUN ["npm", "ci"]
RUN ["npm", "run", "build"]

FROM nginx:1.23-alpine
COPY --from=build /app/dist /usr/share/nginx/html
