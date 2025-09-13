cmake_minimum_required(VERSION 3.5)
project(app CXX)

find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GRPC REQUIRED grpc++)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${GRPC_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/protos)

set(PROTO_FILES ${CMAKE_SOURCE_DIR}/protos/biblio.proto)

set(PROTO_SRCS
    ${CMAKE_SOURCE_DIR}/protos/biblio.pb.cc
    ${CMAKE_SOURCE_DIR}/protos/biblio.grpc.pb.cc
)

set(PROTO_HDRS
    ${CMAKE_SOURCE_DIR}/protos/biblio.pb.h
    ${CMAKE_SOURCE_DIR}/protos/biblio.grpc.pb.h
)

add_executable(server server.cpp ${PROTO_SRCS})
target_link_libraries(server ${Protobuf_LIBRARIES} ${GRPC_LIBRARIES})