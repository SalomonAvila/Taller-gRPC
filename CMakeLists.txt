#Por si no han usado CMake les dejo estos comentarios para que puedan entender
#un poquito como y por que esto está funcionando


cmake_minimum_required(VERSION 3.16)
#Este es el nombre del proyecto, realmente no tiene nada de importante eso se puede quitar
project(Taller-RMI-gRPC)

#El estndr con el cual se va a compilar C++, en este caso es la version 17 pero pues pueden cambiarla si quieren
set(CMAKE_CXX_STANDARD 17)

#Busca los paquetes necesarios
find_package(PkgConfig REQUIRED)
pkg_check_modules(GRPC REQUIRED grpc++)

#Esto es para poder generar el ejecutable del servidor. Aca se linkean los archivos que son necesarios para que pueda servir
add_executable(server 
    src/server.cc
    src/suma.pb.cc
    src/suma.grpc.pb.cc
)
#Busca los archivos .h y los linkea
target_include_directories(server PRIVATE ${GRPC_INCLUDE_DIRS} src/)
target_link_libraries(server ${GRPC_LIBRARIES} protobuf)

#Esto es para poder generar el ejecutable del cliente. Aca se linkean los archivos que son necesarios para que pueda servir
add_executable(client 
    src/client.cc
    src/suma.pb.cc
    src/suma.grpc.pb.cc
)
#Busca los archivos .h y los linkea
target_include_directories(client PRIVATE ${GRPC_INCLUDE_DIRS} src/)
target_link_libraries(client ${GRPC_LIBRARIES} protobuf)
