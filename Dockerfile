
#Usa la version 22.04 de ubuntu. tambien se podría 
FROM ubuntu:22.04 AS ubuntu-grpc


#NO va a mostrar cosas interactivas cuando se estén instalando las dependencias de mas abajo
ENV DEBIAN_FRONTEND=noninteractive

#Se instalan las cosas que se necesitan de grpc y otras herramientas
#La idea es que aca estén todas las herramientas que se van a usar
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

#ESs como si estableceria la carpeta /app como la raiz (app es el nombre de la carpet donde van  meter el dockerfiles y CMakeLists.txts)
WORKDIR /app

#Copia al contenedor los archivos que tenga la carpeta
COPY . .

#Se crea una carpeta y se corre un comando de cmake. Este compile_comamands se genera y lers sirve pr el intellisense
#Copia el json de compile_commands al "root" (WORKDIR)
RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. && \
    cp compile_commands.json ..

#Abre el puerto 50051
EXPOSE 50051

#Te mete de una vez en una terminal en la que puedes ejecutar las cosas dentro del contenedor-
CMD ["/bin/bash"]