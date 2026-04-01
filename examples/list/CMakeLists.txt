add_executable(ListExample "${CMAKE_CURRENT_SOURCE_DIR}/lab_list.cpp")
target_sources(
  ListExample PRIVATE FILE_SET HEADERS BASE_DIRS "${CMAKE_SOURCE_DIR}/include" FILES
                      "${CMAKE_SOURCE_DIR}/include/lab/containers/list.hpp"
)
set_target_properties(ListExample PROPERTIES OUTPUT_NAME "list-example")
