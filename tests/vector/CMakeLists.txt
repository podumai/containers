add_executable(VectorTest "${CMAKE_CURRENT_SOURCE_DIR}/vector.cpp")
target_sources(
  VectorTest
  PRIVATE FILE_SET
          HEADERS
          BASE_DIRS
          "${CMAKE_SOURCE_DIR}/include"
          FILES
          "${CMAKE_SOURCE_DIR}/include/lab/containers/vector.hpp"
          "${CMAKE_SOURCE_DIR}/include/lab/containers/assert.hpp"
)
target_link_libraries(VectorTest PRIVATE GTest::gtest GTest::gtest_main)
target_compile_options(VectorTest PRIVATE ${SANITIZER_FLAGS})
target_link_options(VectorTest PRIVATE ${SANITIZER_FLAGS})
set_target_properties(VectorTest PROPERTIES OUTPUT_NAME "vector-test")

gtest_discover_tests(VectorTest)
