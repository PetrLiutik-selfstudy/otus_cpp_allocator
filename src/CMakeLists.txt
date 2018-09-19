cmake_minimum_required(VERSION 3.2)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(allocator VERSION 1.1.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(allocator VERSION 1.1.1)
endif ()

# Setup application
add_executable(${PROJECT_NAME} main.cpp
        custom_heap.cpp
        ver.cpp
        ../inc/custom_heap.h
        ../inc/custom_allocator.h
        ../inc/custom_vector.h
        ../inc/factorial.h
        ../inc/ver.h)

configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_CONTACT petr_lutik@mail.ru)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

include (CPack)