find_package(Catch2 3 REQUIRED)
include(CTest)
include(Catch)

add_executable(ForwardListTest)
target_sources(
  ForwardListTest
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/forward_list.cpp"
)
target_link_libraries(
  ForwardListTest
  PRIVATE
  ForwardListModule::ForwardListModule
  Catch2::Catch2
  Catch2::Catch2WithMain
)
target_compile_features(
  ForwardListTest
  PRIVATE
  cxx_std_23
)
set_target_properties(
  ForwardListTest
  PROPERTIES
  OUTPUT_NAME "forward-list-test"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)

catch_discover_tests(ForwardListTest)
