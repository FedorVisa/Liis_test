cmake_minimum_required(VERSION 3.25)
project(niis)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})


add_executable(niis main.cpp Api_reader.hpp Mosquitto_mqtt.hpp
        MyException.hpp)

target_link_libraries(niis ${Boost_LIBRARIES})
target_link_libraries(niis ${OPENSSL_LIBRARIES})
target_link_libraries(niis mosquitto)