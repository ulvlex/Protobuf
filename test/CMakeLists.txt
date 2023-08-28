cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(my_test)

set(CMAKE_CXX_STANDARD 14)

#GTEST
set(GTEST_MAIN_LIBRARY ${GTEST_LIBRARY})
find_package(GTest REQUIRED)
add_test(test ${EXECUTABLE_OUTPUT_PATH}/my_test)

#Protobuf
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(my_test
        Running_tests.cpp
        Test_DelimitedMessagesStreamParser.cpp
        Test_Helpers.cpp
        ${PROTO_SRCS}
        ${PROTO_HDRS}     	${CMAKE_SOURCE_DIR}/src/protobuf_parser/DelimitedMessagesStreamParser.hpp)

target_link_libraries(my_test PRIVATE
    ${Protobuf_LIBRARIES}
    ${GTEST_LIBRARIES}
)
