cmake_minimum_required(VERSION 3.26.0)

project(EXECUTE VERSION 1.0)
add_executable(EXECUTE Source.cpp)

target_include_directories(EXECUTE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)