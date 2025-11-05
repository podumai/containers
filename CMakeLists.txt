cmake_minimum_required(VERSION 3.30)
project(
  Containers
  VERSION 0.0.1
  LANGUAGES CXX
  DESCRIPTION "Laboratory work about containers"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(DEFAULT_BUILD_TYPE "Release" CACHE INTERNAL "Default project build type")
if(NOT CMAKE_BUILD_TYPE)
  message(WARNING "[${PROJECT_NAME}] CMAKE_BUILD_TYPE not configured. Current: ${DEFAULT_BUILD_TYPE}")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  set_property(
    CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
  )
endif()

if($ENV{DEV})
  message(CHECK_START "[${PROJECT_NAME}] Detecting clang-tidy...")
  find_program(CLANG_TIDY_EXE "clang-tidy")
  if(CLANG_TIDY_EXE)
    message(CHECK_PASS "found")
    message(STATUS "[${PROJECT_NAME}] Configuring clang-tidy command")
    set(CMAKE_CXX_CLANG_TIDY "$<$<CONFIG:Debug>:${CLANG_TIDY_EXE};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy>")
    message(STATUS "[${PROJECT_NAME}] clang-tidy setupped")
  else()
    message(CHECK_FAIL "not found")
    message(WARNING "[${PROJECT_NAME}] Configuration processes further. Build will not support static analysis")
  endif()

  message(CHECK_START "[${PROJECT_NAME}] Detecting ccache...")
  find_program(CCACHE_EXE "ccache")
  if(CCACHE_EXE)
    message(CHECK_PASS "found")
    message(STATUS "[${PROJECT_NAME}] Configuring CMake to use ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXE}")
    message(STATUS "[${PROJECT_NAME}] ccache setupped")
  else()
    message(CHECK_FAIL "not found")
    message(WARNING "[${PROJECT_NAME}] Configuration processes further without ccache support")
  endif()
endif()

add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(modules)
