if(ENABLE_SANITIZER)
  set(SANITIZER_FLAGS $<$<AND:$<CXX_COMPILER_ID:Clang,GNU,AppleClang>,$<CONFIG:Debug>>:-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment>)
else()
  set(SANITIZER_FLAGS "")
endif()

add_subdirectory(forward_list)
add_subdirectory(list)
add_subdirectory(vector)
