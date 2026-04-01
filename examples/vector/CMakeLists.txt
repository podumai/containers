add_executable(VectorExample)
target_sources(
  VectorExample
  PRIVATE FILE_SET
          HEADERS
          BASE_DIRS
          "${CMAKE_SOURCE_DIR}/include"
          FILES
          "${CMAKE_SOURCE_DIR}/include/lab/containers/vector.hpp"
          "${CMAKE_CURRENT_SOURCE_DIR}/lab_vector.cpp"
)
set_target_properties(VectorExample PROPERTIES OUTPUT_NAME "vector-example")
