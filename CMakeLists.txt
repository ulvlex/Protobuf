cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(Protobuf_parser)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS message.proto)

add_executable(Protobuf_parser
	src/main.cpp
	src/protobuf_parser/DelimitedMessagesStreamParser.hpp
	src/protobuf_parser/Helpers.hpp 
	${PROTO_SRCS} 
	${PROTO_HDRS})

target_link_libraries(Protobuf_parser PRIVATE ${Protobuf_LIBRARIES})

enable_testing()
add_subdirectory(test)
