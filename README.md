#  Taller gRPC – Instrucciones de ejecución
Salomón Avila, Camila Montealegre y Jorge Olaya

## 1️. Instalar dependencias (Linux)
```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config autoconf automake libtool git
sudo apt install -y libgrpc++-dev libprotobuf-dev protobuf-compiler protobuf-compiler-grpc
```
---

## 2️. Compilar el proyecto
```bash
mkdir -p build
cd build
cmake ..
make -j
```

Se generan dos ejecutables:
- `./server`
- `./client`

---

## 3️. Ejecutar el servidor
En una terminal:
```bash
./server
```
El servidor escucha por defecto en `0.0.0.0:50051`.

---

## 4️. Ejecutar el cliente
En otra terminal:
```bash
./client
```

Si el servidor está en otra máquina o puerto:
```bash
./client <host>:<puerto>
```
Ejemplo:
```bash
./client 192.168.1.50:50051
```
