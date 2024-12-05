cmake_minimum_required(VERSION 3.29)
project(cway C)

set(CMAKE_C_STANDARD 17)

# Find GLib package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

include_directories(${})
file(GLOB LIB_SOURCES "src/*.c")

add_executable(cway main.c ${LIB_SOURCES})

# Link GLib libraries
include_directories(${GLIB_INCLUDE_DIRS})
target_link_libraries(cway ${GLIB_LIBRARIES})
# Add ./lib folder to the include path

# Add a custom target to run the executable
add_custom_target(run
  COMMAND cway
  DEPENDS cway
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
