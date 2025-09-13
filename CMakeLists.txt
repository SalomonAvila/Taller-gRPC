cmake_minimum_required(VERSION 3.5)
project(app CXX)

find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GRPC REQUIRED grpc++)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${GRPC_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/protos)

set(PROTO_FILES ${CMAKE_SOURCE_DIR}/protos/biblio.proto)

# Archivos generados (queremos que estén en protos/)
set(PROTO_SRCS
    ${CMAKE_SOURCE_DIR}/protos/biblio.pb.cc
    ${CMAKE_SOURCE_DIR}/protos/biblio.grpc.pb.cc
)

set(PROTO_HDRS
    ${CMAKE_SOURCE_DIR}/protos/biblio.pb.h
    ${CMAKE_SOURCE_DIR}/protos/biblio.grpc.pb.h
)

# Generar código desde el proto, guardando en protos/
add_custom_command(
    OUTPUT ${PROTO_SRCS} ${PROTO_HDRS}
    COMMAND protoc
    ARGS --proto_path=${CMAKE_SOURCE_DIR}/protos
         --cpp_out=${CMAKE_SOURCE_DIR}/protos
         --grpc_out=${CMAKE_SOURCE_DIR}/protos
         --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin
         ${PROTO_FILES}
    DEPENDS ${PROTO_FILES}
)

# Ejecutables
add_executable(server server.cpp ${PROTO_SRCS})
target_link_libraries(server ${Protobuf_LIBRARIES} ${GRPC_LIBRARIES})