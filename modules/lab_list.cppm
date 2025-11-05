module;

#include <concepts>
#include <iostream>
#include <memory>
#include <type_traits>

export module lab_list;

export {
  auto Hello() -> void { std::cout << "Hello, world!" << std::endl; }
}
