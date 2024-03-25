cmake_minimum_required(VERSION 3.10)
project(BaBP)
include(CTest)

set(COMMON_SOURCES)

set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,undefined -Wall -Wextra")
set(CMAKE_CXX_FLAGS "-std=c++17")

add_executable(
    BaBP ${COMMON_SOURCES} 
    src/main/platform.cpp
    src/main/inflater.cpp
)

target_include_directories(BaBP
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/include
    $ENV{BOOST_PATH}
    $ENV{EIGEN_PATH}
)