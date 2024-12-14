#ifndef BASE_UTILITY_HPP
#define BASE_UTILITY_HPP

#include "base_utility_macros.hpp"

#include <array>
#include <vector>

#ifdef USE_STD_COPY
#include <algorithm>
#else  // USE_STD_COPY
#endif // USE_STD_COPY

namespace Base {
namespace Utility {

template <typename T> inline T avoid_zero_divide(T in, T division_min) {
  if (in < division_min) {
    if (in >= 0) {
      return division_min;
    } else if (in > -division_min) {
      return -division_min;
    }
  }

  return in;
}

template <typename T> inline bool near_zero(T in, T division_min) {
  bool flag = false;
  if (in < division_min) {
    if (in >= 0) {
      flag = true;
    } else if (in > -division_min) {
      flag = true;
    }
  }

  return flag;
}

template <typename T> inline void swap_value(T &in_1, T &in_2) {
  T temp;
  temp = in_1;
  in_1 = in_2;
  in_2 = temp;
}

template <typename T> inline T sign(T x) {
  if (x >= 0) {
    return static_cast<T>(1);
  } else {
    return static_cast<T>(-1);
  }
}

template <typename T, std::size_t N>
inline void copy(std::vector<T> &destination, std::vector<T> &source) {

#ifdef USE_STD_COPY

  for (std::size_t i = 0; i < N; i++) {
    destination[i] = source[i];
  }

#else // USE_STD_COPY

  std::copy(source.begin(), source.end(), destination.begin());

#endif // USE_STD_COPY
}

template <typename T, std::size_t N>
inline void copy(std::array<T, N> &destination, std::array<T, N> &source) {

#ifdef USE_STD_COPY

  for (std::size_t i = 0; i < N; i++) {
    destination[i] = source[i];
  }

#else // USE_STD_COPY

  std::copy(source.begin(), source.end(), destination.begin());

#endif // USE_STD_COPY
}

} // namespace Utility
} // namespace Base

#endif // BASE_UTILITY_HPP
