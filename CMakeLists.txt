cmake_minimum_required(VERSION 3.5)
project(rclquick)

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

#########################
## Find Rcl

find_package(ament_cmake  REQUIRED)
find_package(rcl          REQUIRED)
find_package(ament_index_cpp REQUIRED)

#########################
## Find Qt6

find_package(Qt6          COMPONENTS Quick Concurrent REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

#########################
## Define qml install dir

set(INSTALL_QML_DIR     lib/qt6/qml CACHE PATH "Installation directory for libraries")

# Make relative paths absolute
if(NOT IS_ABSOLUTE "${INSTALL_QML_DIR}")
  set(INSTALL_QML_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_QML_DIR}")
endif()

####################
## environment hooks
ament_environment_hooks(env_hook/rclquick.sh.in)

ament_package()

########################
## subdirectories

add_subdirectory(Rcl)

