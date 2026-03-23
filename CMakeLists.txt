cmake_minimum_required(VERSION 3.20)
project(DeterministicPhysicsSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

find_package(SDL3 CONFIG QUIET)
if(NOT SDL3_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
endif()

set(SIMULATOR_SOURCES
  src/Application.cpp
  src/Renderer.cpp
  src/SimulationFactory.cpp
  src/physics/Ball.cpp
  src/physics/PhysicsEngine.cpp
  src/physics/Simulation.cpp
  src/physics/Wall.cpp
)

add_library(simulator_core ${SIMULATOR_SOURCES})
target_include_directories(simulator_core PUBLIC include)
if(SDL3_FOUND)
  target_link_libraries(simulator_core PUBLIC SDL3::SDL3)
else()
  target_link_libraries(simulator_core PUBLIC PkgConfig::SDL3)
endif()
target_compile_options(simulator_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(physics_simulator src/main.cpp)
target_link_libraries(physics_simulator PRIVATE simulator_core)

add_executable(physics_tests tests/PhysicsTests.cpp)
target_link_libraries(physics_tests PRIVATE simulator_core)

if(BUILD_TESTING)
  add_test(NAME physics_tests COMMAND physics_tests)
endif()
