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

/* copy vector */
template <typename T, std::size_t N, std::size_t Index> struct VectorCopyCore {
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {
    destination[Index] = source[Index];
    VectorCopyCore<T, N, Index - 1>::compute(source, destination);
  }
};

template <typename T, std::size_t N> struct VectorCopyCore<T, N, 0> {
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {
    destination[0] = source[0];
  }
};

template <typename T, std::size_t N>
static inline void COMPILED_VECTOR_COPY(const std::vector<T> &source,
                                        std::vector<T> &destination) {
  VectorCopyCore<T, N, N - 1>::compute(source, destination);
}

template <typename T, std::size_t N>
inline void copy(const std::vector<T> &source, std::vector<T> &destination) {

#ifdef USE_STD_COPY

  std::copy(source.begin(), source.end(), destination.begin());

#else // USE_STD_COPY

  Base::Utility::COMPILED_VECTOR_COPY<T, N>(source, destination);

#endif // USE_STD_COPY
}

/* copy vector part */
template <typename T, std::size_t Source_Start, std::size_t Destination_Start,
          std::size_t Index>
struct VectorCopyPartCore {
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {
    destination[Index + Destination_Start - 1] =
        source[Index + Source_Start - 1];
    VectorCopyPartCore<T, Source_Start, Destination_Start, Index - 1>::compute(
        source, destination);
  }
};

template <typename T, std::size_t Source_Start, std::size_t Destination_Start>
struct VectorCopyPartCore<T, Source_Start, Destination_Start, 0> {
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

template <typename T, std::size_t Source_Start, std::size_t Copy_Size,
          std::size_t Destination_Start, std::size_t Source_Size,
          std::size_t Destination_Size>
static inline void COMPILED_VECTOR_COPY_PART(const std::vector<T> &source,
                                             std::vector<T> &destination) {
  VectorCopyPartCore<T, Source_Start, Destination_Start, Copy_Size>::compute(
      source, destination);
}

template <typename T, std::size_t Source_Start, std::size_t Copy_Size,
          std::size_t Destination_Start, std::size_t Source_Size,
          std::size_t Destination_Size>
inline void copy(const std::vector<T> &source, std::vector<T> &destination) {

#ifdef USE_STD_COPY

  std::copy(source.begin() + Source_Start,
            source.begin() + Source_Start + Copy_Size,
            destination.begin() + Destination_Start);

#else // USE_STD_COPY

  Base::Utility::COMPILED_VECTOR_COPY_PART<T, Source_Start, Copy_Size,
                                           Destination_Start, Source_Size,
                                           Destination_Size>(source,
                                                             destination);

#endif // USE_STD_COPY
}

/* copy array */
template <typename T, std::size_t N, std::size_t Index> struct ArrayCopyCore {
  static void compute(const std::array<T, N> &source,
                      std::array<T, N> &destination) {
    destination[Index] = source[Index];
    ArrayCopyCore<T, N, Index - 1>::compute(source, destination);
  }
};

template <typename T, std::size_t N> struct ArrayCopyCore<T, N, 0> {
  static void compute(const std::array<T, N> &source,
                      std::array<T, N> &destination) {
    destination[0] = source[0];
  }
};

template <typename T, std::size_t N>
static inline void COMPILED_ARRAY_COPY(const std::array<T, N> &source,
                                       std::array<T, N> &destination) {
  ArrayCopyCore<T, N, N - 1>::compute(source, destination);
}

template <typename T, std::size_t N>
inline void copy(const std::array<T, N> &source,
                 std::array<T, N> &destination) {

#ifdef USE_STD_COPY

  std::copy(source.begin(), source.end(), destination.begin());

#else // USE_STD_COPY

  Base::Utility::COMPILED_ARRAY_COPY<T, N>(source, destination);

#endif // USE_STD_COPY
}

/* copy array part */
template <typename T, std::size_t Source_Size, std::size_t Destination_Size,
          std::size_t Source_Start, std::size_t Destination_Start,
          std::size_t Index>
struct ArrayCopyPartCore {
  static void compute(const std::array<T, Source_Size> &source,
                      std::array<T, Destination_Size> &destination) {
    destination[Index + Destination_Start - 1] =
        source[Index + Source_Start - 1];
    ArrayCopyPartCore<T, Source_Size, Destination_Size, Source_Start,
                      Destination_Start, Index - 1>::compute(source,
                                                             destination);
  }
};

template <typename T, std::size_t Source_Size, std::size_t Destination_Size,
          std::size_t Source_Start, std::size_t Destination_Start>
struct ArrayCopyPartCore<T, Source_Size, Destination_Size, Source_Start,
                         Destination_Start, 0> {
  static void compute(const std::array<T, Source_Size> &source,
                      std::array<T, Destination_Size> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

template <typename T, std::size_t Source_Start, std::size_t Copy_Size,
          std::size_t Destination_Start, std::size_t Source_Size,
          std::size_t Destination_Size>
static inline void
COMPILED_ARRAY_COPY_PART(const std::array<T, Source_Size> &source,
                         std::array<T, Destination_Size> &destination) {
  ArrayCopyPartCore<T, Source_Size, Destination_Size, Source_Start,
                    Destination_Start, Copy_Size>::compute(source, destination);
}

template <typename T, std::size_t Source_Start, std::size_t Copy_Size,
          std::size_t Destination_Start, std::size_t Source_Size,
          std::size_t Destination_Size>
inline void copy(const std::array<T, Source_Size> &source,
                 std::array<T, Destination_Size> &destination) {

#ifdef USE_STD_COPY

  std::copy(source.begin() + Source_Start,
            source.begin() + Source_Start + Copy_Size,
            destination.begin() + Destination_Start);

#else // USE_STD_COPY

  Base::Utility::COMPILED_ARRAY_COPY_PART<T, Source_Start, Copy_Size,
                                          Destination_Start, Source_Size,
                                          Destination_Size>(source,
                                                            destination);

#endif // USE_STD_COPY
}

} // namespace Utility
} // namespace Base

#endif // BASE_UTILITY_HPP
