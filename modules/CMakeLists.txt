set(LAB_MODULES_PATH "${CMAKE_SOURCE_DIR}/modules" CACHE INTERNAL "Helper variable for modules path")
set(LAB_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/include" CACHE INTERNAL "Helper variable for include path")

add_library(LabMacroHelpers INTERFACE)
add_library(LabMacroHelpers::LabMacroHelpers ALIAS LabMacroHelpers)
target_sources(
  LabMacroHelpers
  INTERFACE
  FILE_SET HEADERS
  BASE_DIRS "${LAB_INCLUDE_PATH}"
  FILES
  "${LAB_INCLUDE_PATH}/tpu/helper_macros.hpp"
  "${LAB_INCLUDE_PATH}/tpu/modules/module_helper_macros.hpp"
)

add_library(ForwardListModule)
add_library(ForwardListModule::ForwardListModule ALIAS ForwardListModule)
target_sources(
  ForwardListModule
  PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS "${LAB_MODULES_PATH}"
  FILES "${LAB_MODULES_PATH}/lab_forward_list.cppm"
)
target_compile_features(
  ForwardListModule
  PRIVATE
  cxx_std_23
)
target_link_libraries(
  ForwardListModule
  PRIVATE
  LabMacroHelpers::LabMacroHelpers
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
target_link_libraries(
  ListModule
  PRIVATE
  LabMacroHelpers::LabMacroHelpers
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
