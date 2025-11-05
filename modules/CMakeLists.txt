set(LAB_MODULES_PATH "${CMAKE_SOURCE_DIR}/modules" CACHE INTERNAL "Helper variable for modules path")
set(LAB_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/include" CACHE INTERNAL "Helper variable for include path")

add_library(ForwardListModule)
add_library(ForwardListModule::ForwardListModule ALIAS ForwardListModule)
target_sources(
  ForwardListModule
  PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS "${LAB_MODULES_PATH}"
  FILES "${LAB_MODULES_PATH}/lab_forward_list.cppm"
  PRIVATE
  FILE_SET HEADERS
  BASE_DIRS "${CMAKE_SOURCE_DIR}/include"
  FILES
  "${CMAKE_SOURCE_DIR}/include/tpu/modules/module_helper_macros.hpp"
  "${CMAKE_SOURCE_DIR}/include/tpu/helper_macros.hpp"
)
target_compile_features(
  ForwardListModule
  PRIVATE
  cxx_std_23
)

add_library(ListModule)
add_library(ListModule::ListModule ALIAS ListModule)
target_sources(
  ListModule
  PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS "${LAB_MODULES_PATH}"
  FILES "${LAB_MODULES_PATH}/lab_list.cppm"
)
target_compile_features(
  ListModule
  PRIVATE
  cxx_std_23
)

add_library(VectorModule)
add_library(VectorModule::VectorModule ALIAS VectorModule)
target_sources(
  VectorModule
  PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS "${LAB_MODULES_PATH}"
  FILES "${LAB_MODULES_PATH}/lab_vector.cppm"
)
target_compile_features(
  VectorModule
  PRIVATE
  cxx_std_23
)
