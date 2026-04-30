FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y g++

WORKDIR /app
COPY hello.cpp .

RUN g++ -static -o hello hello.cpp

FROM alpine:latest

COPY --from=builder /app/hello /hello

CMD ["/hello"]