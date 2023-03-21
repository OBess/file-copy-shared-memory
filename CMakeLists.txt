cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_TARGET_TRIPLET x64-windows-static)
set(VCPKG_BUILD_TYPE x64)

project(file-copy-shared-memory VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/deploy)

set(Boost_NO_WARN_NEW_VERSIONS 1)
find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(file-copy-shared-memory main.cpp)

if (MSVC)
    set_target_properties(file-copy-shared-memory PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
endif()

target_include_directories(file-copy-shared-memory PRIVATE include ${Boost_INCLUDE_DIR})
target_link_libraries(file-copy-shared-memory PRIVATE Boost::program_options)