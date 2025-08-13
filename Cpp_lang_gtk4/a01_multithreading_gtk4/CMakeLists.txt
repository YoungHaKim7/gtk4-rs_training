cmake_minimum_required(VERSION 3.28)
project(a01_multithreading_gtk4 LANGUAGES CXX)

# Check for a generator that supports C++ modules
if (NOT CMAKE_CXX_COMPILER_SUPPORTS_MODULES)
    message(FATAL_ERROR "This project uses C++20 modules, which require a CMake generator that supports them (e.g., Ninja, Ninja Multi-Config, or Visual Studio 17.4+). Please re-run CMake with a supported generator, for example: cmake -G Ninja ..")
endif()

# Enable C++20 with modules
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON) # Explicitly enable module scanning

# On GCC/Clang, enable experimental modules support flags where needed
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-fmodules-ts)
endif()

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Try to find GPHOTO2 using pkg-config first
pkg_check_modules(GPHOTO2_PC QUIET libgphoto2)
if (GPHOTO2_PC_FOUND)
    set(GPHOTO2_INCLUDE_DIR ${GPHOTO2_PC_INCLUDE_DIRS})
    set(GPHOTO2_LIBRARY ${GPHOTO2_PC_LIBRARIES})
    message(STATUS "Found GPHOTO2 via pkg-config.")
else()
    # Fallback to find_path/find_library if pkg-config fails
    find_path(GPHOTO2_INCLUDE_DIR gphoto2/gphoto2-camera.h)
    find_library(GPHOTO2_LIBRARY gphoto2)
    if (NOT GPHOTO2_INCLUDE_DIR OR NOT GPHOTO2_LIBRARY)
        message(FATAL_ERROR "GPHOTO2 not found. Please install libgphoto2-dev or ensure it's in your PKG_CONFIG_PATH/library paths.")
    endif()
    message(STATUS "Found GPHOTO2 via find_path/find_library.")
endif()


# Find WIRINGPI (optional)
find_path(WIRINGPI_INCLUDE_DIR wiringPi.h)
find_library(WIRINGPI_LIBRARY wiringPi)

if (WIRINGPI_INCLUDE_DIR AND WIRINGPI_LIBRARY)
    message(STATUS "Found WIRINGPI.")
    set(WIRINGPI_FOUND TRUE)
else()
    message(STATUS "WIRINGPI not found. It will be skipped. If required, please install wiringPi-dev or ensure it's in your library paths.")
    set(WIRINGPI_FOUND FALSE)
endif()

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
  $<$<BOOL:${WIRINGPI_FOUND}>:${WIRINGPI_INCLUDE_DIR}> # Only add if found
)

# Link libraries
target_link_libraries(a01_multithreading_gtk4 PRIVATE
  ${GTK3_LIBRARIES}
  ${GPHOTO2_LIBRARY}
  $<$<BOOL:${WIRINGPI_FOUND}>:${WIRINGPI_LIBRARY}> # Only link if found
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