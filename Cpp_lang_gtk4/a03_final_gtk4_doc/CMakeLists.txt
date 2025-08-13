cmake_minimum_required(VERSION 3.28)
project(a03_final_gtk4_doc LANGUAGES CXX)

# Enable C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM4 REQUIRED gtkmm-4.0)

find_package(Threads REQUIRED)

# Sources
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(a03_final_gtk4_doc
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/helloworld.cpp
)

# Include directories
target_include_directories(a03_final_gtk4_doc PRIVATE
  ${GTKMM4_INCLUDE_DIRS}
  ${SRC_DIR}/headers
)

# Link libraries
target_link_libraries(a03_final_gtk4_doc PRIVATE
  ${GTKMM4_LIBRARIES}
  Threads::Threads
)

# Compiler flags for packages
target_compile_options(a03_final_gtk4_doc PRIVATE
  ${GTKMM4_CFLAGS_OTHER}
)

# Linker flags for packages
target_link_options(a03_final_gtk4_doc PRIVATE
  ${GTKMM4_LDFLAGS_OTHER}
)
