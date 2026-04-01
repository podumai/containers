add_executable(ForwardListExample)
target_sources(
  ForwardListExample
  PRIVATE FILE_SET
          HEADERS
          BASE_DIRS
          "${CMAKE_SOURCE_DIR}/include"
          FILES
          "${CMAKE_SOURCE_DIR}/include/lab/containers/forward_list.hpp"
          "${CMAKE_CURRENT_SOURCE_DIR}/lab_forward_list.cpp"
)
set_target_properties(ForwardListExample PROPERTIES OUTPUT_NAME "forward-list-example")
