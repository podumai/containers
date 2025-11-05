add_executable(ForwardListExample)
target_sources(
  ForwardListExample
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/lab_forward_list.cpp"
)
target_compile_features(
  ForwardListExample
  PRIVATE
  cxx_std_23
)
target_link_libraries(
  ForwardListExample
  PRIVATE
  ForwardListModule::ForwardListModule
)
set_target_properties(
  ForwardListExample
  PROPERTIES
  OUTPUT_NAME "forward-list-example"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
