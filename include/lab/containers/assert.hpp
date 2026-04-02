#pragma once

#include <cassert>
#include <string_view>
#include <iostream>

namespace lab::utility {

inline auto LabContainersAssert(bool condition, std::string_view message) -> void {
  #ifndef NDEBUG
  if (!condition) {
    std::cerr << message << '\n';
    assert(false);
  }
  #endif
}

}
