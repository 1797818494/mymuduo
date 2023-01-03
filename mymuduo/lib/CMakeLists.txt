cmake_minimum_required(VERSION 2.5)
project(mymuduo)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")
aux_source_directory(. SRC_LIST)
#编译生成mymuduo
add_library(mymuduo SHARED ${SRC_LIST})
