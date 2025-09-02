FROM ubuntu:22.04 AS ubuntu-grpc



ENV DEBIAN_FRONTEND=noninteractive


RUN apt-get update && apt-get install -y \
    libgrpc++-dev \
    libgrpc-dev \
    libprotobuf-dev \
    protobuf-compiler-grpc \
    pkg-config \
    build-essential \
    cmake \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

WORKDIR /app

COPY . .

RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. && \
    cp compile_commands.json .. || \
    echo '[{"directory": "/app", "command": "/usr/bin/c++ -std=c++17 -I/usr/include src/server.cc -lgrpc++ -lgrpc -lprotobuf", "file": "src/server.cc"}]' > ../compile_commands.json

EXPOSE 50051

CMD ["/bin/bash"]