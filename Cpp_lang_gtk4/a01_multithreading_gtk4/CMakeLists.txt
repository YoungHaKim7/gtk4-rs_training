cmake_minimum_required(VERSION 3.28)
project(a01_multithreading_gtk4 LANGUAGES CXX)

# Enable C++20 with modules
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# On GCC/Clang, enable experimental modules support flags where needed
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-fmodules-ts)
endif()

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

find_path(GPHOTO2_INCLUDE_DIR gphoto2/gphoto2-camera.h)
find_library(GPHOTO2_LIBRARY gphoto2)

find_library(WIRINGPI_LIBRARY wiringPi)

find_package(Threads REQUIRED)

# Sources
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MOD_DIR ${SRC_DIR}/modules)

add_executable(a01_multithreading_gtk4 ${SRC_DIR}/main.cpp)

# Register C++20 module sources so CMake handles BMI order
target_sources(a01_multithreading_gtk4
  PUBLIC
    FILE_SET CXX_MODULES FILES
      ${MOD_DIR}/capture.gui.ixx
      ${MOD_DIR}/capture.cameras.ixx
      ${MOD_DIR}/capture.app.ixx
)

# Include directories
target_include_directories(a01_multithreading_gtk4 PRIVATE
  ${GTK3_INCLUDE_DIRS}
  ${GPHOTO2_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(a01_multithreading_gtk4 PRIVATE
  ${GTK3_LIBRARIES}
  ${GPHOTO2_LIBRARY}
  ${WIRINGPI_LIBRARY}
  Threads::Threads
)

# Compiler flags for packages
target_compile_options(a01_multithreading_gtk4 PRIVATE
  ${GTK3_CFLAGS_OTHER}
)

# Linker flags for packages
target_link_options(a01_multithreading_gtk4 PRIVATE
  ${GTK3_LDFLAGS_OTHER}
)
