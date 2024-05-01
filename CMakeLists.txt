cmake_minimum_required(VERSION 3.10)
project(BaBP)
include(CTest)

set(
    COMMON_SOURCES
    src/main/backend.cpp
    src/main/conjugate_coord_descent.cpp
    src/main/eigen_ext.cpp
    src/main/index_bound_set.cpp
    src/main/inflater.cpp
    src/main/operations.cpp
    src/main/tasks.cpp
    src/main/utils.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,undefined -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-std=c++17")

add_executable(
    BaBP ${COMMON_SOURCES} 
    src/main/platform.cpp
)

target_include_directories(BaBP
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/include
    $ENV{BOOST_PATH}
    $ENV{EIGEN_PATH}
)

# ----------- Test Section -----------
# add_executable(compute_test)
# target_sources(compute_test
#     PRIVATE
#     ${PROJECT_SOURCE_DIR}/src/tests/compute_test.cpp
#     ${COMMON_SOURCES}
# )

# target_include_directories(compute_test
#     PRIVATE
#     ${PROJECT_SOURCE_DIR}/src/include
#     $ENV{BOOST_PATH}
#     $ENV{EIGEN_PATH}
# )

# add_test(NAME compute_t COMMAND compute_test)