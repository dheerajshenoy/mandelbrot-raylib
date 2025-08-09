cmake_minimum_required(VERSION 3.16)

project(mandelbrot-raylib
    VERSION 0.1
    DESCRIPTION "Mandelbrot Visualizer"
    LANGUAGES CXX
)

# Modern C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type default (Release if not specified)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the build type: Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif()

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Debug/Release specific flags
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

find_package(raylib REQUIRED)


# set(HEADERS)
    # src/mb.hpp)

# Sources
set(SOURCES
    src/main.cpp
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Example: Link Qt
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
