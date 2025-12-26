#pragma once

#include <cassert>
#include <iostream>

#ifndef NDEBUG
  #define LAB_CONTAINERS_ASSERT(condition, message) \
    do {                                            \
      if (!(condition)) {                           \
        std::cerr << (message) << '\n';             \
      }                                             \
      assert((condition));                          \
    } while (false)
#else
  #define LAB_CONTAINERS_ASSERT(condition, message) \
    (void) int { }
#endif
