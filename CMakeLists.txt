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
    src/main/solution.cpp
    src/main/tasks.cpp
    src/main/utils.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,undefined -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-std=c++17")

set(BOOST_ROOT $ENV{BOOST_PATH})
set(BOOST_DIR "${BOOST_ROOT}/~/Documents/dev/cpp_libs/boost_1_84_0_build")
# set(BOOST_DIR ${BOOST_ROOT}/libs)
set(Boost_USE_STATIC_LIBS   ON)

find_package(Boost 1.84)

add_library(boost_coroutine SHARED IMPORTED)
add_library(boost_context SHARED IMPORTED)

set_target_properties(boost_coroutine PROPERTIES
  IMPORTED_LOCATION "${BOOST_DIR}/lib/libboost_coroutine.so"
  INTERFACE_INCLUDE_DIRECTORIES "${BOOST_DIR}/include/boost/coroutine2"
)
set_target_properties(boost_context PROPERTIES
  IMPORTED_LOCATION "${BOOST_DIR}/lib/libboost_context.so"
  INTERFACE_INCLUDE_DIRECTORIES "${BOOST_DIR}/include/boost/context"
)

add_executable(
    BaBP 
    ${COMMON_SOURCES}
    src/main/platform.cpp
)

target_include_directories(BaBP
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/include
    ${Boost_INCLUDE_DIR}
    $ENV{EIGEN_PATH}
)

target_link_libraries(BaBP boost_coroutine boost_context)

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
    "DEBUG=$<IF:$<CONFIG:Debug>,1,0>"
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