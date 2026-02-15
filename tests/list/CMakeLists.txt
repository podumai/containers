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
target_compile_options(
  ListTest
  PRIVATE
  ${SANITIZER_FLAGS}
)
target_link_options(
  ListTest
  PRIVATE
  ${SANITIZER_FLAGS}
)
set_target_properties(
  ListTest
  PROPERTIES
  OUTPUT_NAME "list-test"
)

catch_discover_tests(ListTest)
