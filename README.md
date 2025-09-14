#  Taller gRPC – Instrucciones de ejecución
Salomón Avila, Camila Montealegre y Jorge Olaya

## 1️. Instalar dependencias (Linux)
```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config autoconf automake libtool git
sudo apt install -y libgrpc++-dev libprotobuf-dev protobuf-compiler protobuf-compiler-grpc
```
---

## 2. Compilar proto (Linux)
```bash
protoc -I=protos --grpc_out=protos --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` protos/biblio.proto
protoc -I=protos --cpp_out=protos protos/biblio.proto
```

## 3. Compilar el proyecto
```bash
mkdir -p build
cd build
cmake ..
make -j
```

Se generan dos ejecutables dentro de la carpeta build:
- `./server`
- `./client`

---

## 3️. Ejecutar el servidor
En una terminal:
```bash
./server
```
El servidor escucha en la direccion `10.43.101.228:50051`.

---

## 4️. Ejecutar el cliente
En otra terminal:
```bash
./client
```