add_executable(ListTest)
target_sources(
  ListTest
  PRIVATE
  FILE_SET HEADERS
  BASE_DIRS "${CMAKE_SOURCE_DIR}/include"
  FILES "${CMAKE_SOURCE_DIR}/include/lab/containers/list.hpp"
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/list.cpp"
)
target_link_libraries(
  ListTest
  PRIVATE
  Catch2::Catch2
  Catch2::Catch2WithMain
)
set_target_properties(
  ListTest
  PROPERTIES
  OUTPUT_NAME "list-test"
)

catch_discover_tests(ListTest)

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
set_target_properties(
  ForwardListTest
  PROPERTIES
  OUTPUT_NAME "forward-list-test"
)

catch_discover_tests(ForwardListTest)

add_executable(VectorTest)
target_sources(
  VectorTest
  PRIVATE
  FILE_SET HEADERS
  BASE_DIRS "${CMAKE_SOURCE_DIR}/include"
  FILES
  "${CMAKE_SOURCE_DIR}/include/lab/containers/vector.hpp"
  "${CMAKE_SOURCE_DIR}/include/lab/containers/assert.hpp"
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/vector.cpp"
)
target_link_libraries(
  VectorTest
  PRIVATE
  GTest::gtest
  GTest::gtest_main
)
set_target_properties(
  VectorTest
  PROPERTIES
  OUTPUT_NAME "vector-test"
)

gtest_discover_tests(VectorTest)
