add_executable(ForwardListTest)
target_sources(
  ForwardListTest
  PRIVATE
  FILE_SET HEADERS
  BASE_DIRS "${CMAKE_SOURCE_DIR}/include"
  FILES "${CMAKE_SOURCE_DIR}/include/lab/containers/forward_list.hpp"
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/forward_list.cpp"
)
target_link_libraries(
  ForwardListTest
  PRIVATE
  Catch2::Catch2
  Catch2::Catch2WithMain
)
target_compile_options(
  ForwardListTest
  PRIVATE
  ${SANITIZER_FLAGS}
)
target_link_options(
  ForwardListTest
  PRIVATE
  ${SANITIZER_FLAGS}
)
set_target_properties(
  ForwardListTest
  PROPERTIES
  OUTPUT_NAME "forward-list-test"
)

catch_discover_tests(ForwardListTest)
