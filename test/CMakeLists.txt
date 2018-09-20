cmake_minimum_required(VERSION 3.2)

# Setup testing
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(${PROJECT_NAME} test_main.cpp
                               ../src/ver.cpp
                               ../src/custom_heap.cpp)

set_target_properties(${PROJECT_NAME}  ${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  INCLUDE_DIRECTORIES ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} gtest pthread )

add_test(ver_test_case ${PROJECT_NAME})
add_test(factorial_test_case ${PROJECT_NAME})
add_test(allocator_test_case ${PROJECT_NAME})
add_test(vector_test_case ${PROJECT_NAME})
