cmake_minimum_required(VERSION 4.0)
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} LANGUAGES CXX)

# Force GCC 15
set(CMAKE_CXX_COMPILER "/opt/gcc-15/bin/g++")

# Enable C++26
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM4 REQUIRED gtkmm-4.0)

find_package(Threads REQUIRED)

# Sources
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${ProjectId}
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/helloworld.cpp
)

# Include directories
target_include_directories(${ProjectId} PRIVATE
  ${GTKMM4_INCLUDE_DIRS}
  ${SRC_DIR}/headers
)

# Link libraries
target_link_libraries(${ProjectId} PRIVATE
  ${GTKMM4_LIBRARIES}
  Threads::Threads
)

# Compiler flags for packages
target_compile_options(${ProjectId} PRIVATE
  ${GTKMM4_CFLAGS_OTHER}
)

# Linker flags for packages
target_link_options(${ProjectId} PRIVATE
  ${GTKMM4_LDFLAGS_OTHER}
)
