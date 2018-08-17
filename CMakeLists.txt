cmake_minimum_required(VERSION 3.2)

project(allocator VERSION 1.1.1)#$ENV{TRAVIS_BUILD_NUMBER})

configure_file(version.h.in version.h)

# Setup application
add_executable(${PROJECT_NAME} main.cpp)

#add_library(ver_lib ver.cpp)
#add_library(allocator_lib allocator.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  #COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

#target_link_libraries(${PROJECT_NAME} ver_lib)
#target_link_libraries(${PROJECT_NAME} allocator_lib)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
