#ifndef __BASE_UTILITY_HPP__
#define __BASE_UTILITY_HPP__

#include "base_utility_macros.hpp"

#include <array>
#include <vector>

#ifdef __BASE_UTILITY_USE_STD_COPY__
#include <algorithm>
#else // __BASE_UTILITY_USE_STD_COPY__

#ifdef __BASE_UTILITY_USE_MEMCPY__
#include <cstring>
#else  // __BASE_UTILITY_USE_MEMCPY__
#endif // __BASE_UTILITY_USE_MEMCPY__

#endif // __BASE_UTILITY_USE_STD_COPY__

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
    destination[Index - 1] = source[Index - 1];
    VectorCopyCore<T, N, Index - 1>::compute(source, destination);
  }
};

template <typename T, std::size_t N> struct VectorCopyCore<T, N, 0> {
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

template <typename T, std::size_t N>
static inline void COMPILED_VECTOR_COPY(const std::vector<T> &source,
                                        std::vector<T> &destination) {
  VectorCopyCore<T, N, N>::compute(source, destination);
}

template <typename T, std::size_t N>
inline void copy(const std::vector<T> &source, std::vector<T> &destination) {

#ifdef __BASE_UTILITY_USE_STD_COPY__

  std::copy(source.begin(), source.end(), destination.begin());

#else // __BASE_UTILITY_USE_STD_COPY__

#ifdef __BASE_UTILITY_USE_MEMCPY__

  std::memcpy(destination.data(), source.data(), N * sizeof(T));

#else // __BASE_UTILITY_USE_MEMCPY__

  Base::Utility::COMPILED_VECTOR_COPY<T, N>(source, destination);

#endif // __BASE_UTILITY_USE_MEMCPY__

#endif // __BASE_UTILITY_USE_STD_COPY__
}

/* copy vector part */
template <typename T, std::size_t Source_Start, std::size_t Destination_Start,
          std::size_t Copy_Size>
struct CheckZeroVectorCopyForStdCopy {
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {

#ifdef __BASE_UTILITY_USE_STD_COPY__

    std::copy(source.begin() + Source_Start,
              source.begin() + Source_Start + Copy_Size,
              destination.begin() + Destination_Start);

#else // __BASE_UTILITY_USE_STD_COPY__

#ifdef __BASE_UTILITY_USE_MEMCPY__

    std::memcpy(destination.data() + Destination_Start,
                source.data() + Source_Start, Copy_Size * sizeof(T));

#else // __BASE_UTILITY_USE_MEMCPY__

    std::copy(source.begin() + Source_Start,
              source.begin() + Source_Start + Copy_Size,
              destination.begin() + Destination_Start);

#endif // __BASE_UTILITY_USE_MEMCPY__

#endif // __BASE_UTILITY_USE_STD_COPY__
  }
};

template <typename T, std::size_t Source_Start, std::size_t Destination_Start>
struct CheckZeroVectorCopyForStdCopy<T, Source_Start, Destination_Start, 0> {
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

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

#ifdef __BASE_UTILITY_USE_STD_COPY__

  CheckZeroVectorCopyForStdCopy<T, Source_Start, Destination_Start,
                                Copy_Size>::compute(source, destination);

#else // __BASE_UTILITY_USE_STD_COPY__

#ifdef __BASE_UTILITY_USE_MEMCPY__

  CheckZeroVectorCopyForStdCopy<T, Source_Start, Destination_Start,
                                Copy_Size>::compute(source, destination);

#else // __BASE_UTILITY_USE_MEMCPY__

  Base::Utility::COMPILED_VECTOR_COPY_PART<T, Source_Start, Copy_Size,
                                           Destination_Start, Source_Size,
                                           Destination_Size>(source,
                                                             destination);

#endif // __BASE_UTILITY_USE_MEMCPY__

#endif // __BASE_UTILITY_USE_STD_COPY__
}

/* copy array */
template <typename T, std::size_t N, std::size_t Index> struct ArrayCopyCore {
  static void compute(const std::array<T, N> &source,
                      std::array<T, N> &destination) {
    destination[Index - 1] = source[Index - 1];
    ArrayCopyCore<T, N, Index - 1>::compute(source, destination);
  }
};

template <typename T, std::size_t N> struct ArrayCopyCore<T, N, 0> {
  static void compute(const std::array<T, N> &source,
                      std::array<T, N> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

template <typename T, std::size_t N>
static inline void COMPILED_ARRAY_COPY(const std::array<T, N> &source,
                                       std::array<T, N> &destination) {
  ArrayCopyCore<T, N, N>::compute(source, destination);
}

template <typename T, std::size_t N>
inline void copy(const std::array<T, N> &source,
                 std::array<T, N> &destination) {

#ifdef __BASE_UTILITY_USE_STD_COPY__

  std::copy(source.begin(), source.end(), destination.begin());

#else // __BASE_UTILITY_USE_STD_COPY__

#ifdef __BASE_UTILITY_USE_MEMCPY__

  std::memcpy(destination.data(), source.data(), N * sizeof(T));

#else // __BASE_UTILITY_USE_MEMCPY__

  Base::Utility::COMPILED_ARRAY_COPY<T, N>(source, destination);

#endif // __BASE_UTILITY_USE_MEMCPY__

#endif // __BASE_UTILITY_USE_STD_COPY__
}

/* copy array part */
template <typename T, std::size_t Source_Size, std::size_t Destination_Size,
          std::size_t Source_Start, std::size_t Destination_Start,
          std::size_t Copy_Size>
struct CheckZeroArrayCopyForStdCopy {
  static void compute(const std::array<T, Source_Size> &source,
                      std::array<T, Destination_Size> &destination) {

#ifdef __BASE_UTILITY_USE_STD_COPY__

    std::copy(source.begin() + Source_Start,
              source.begin() + Source_Start + Copy_Size,
              destination.begin() + Destination_Start);

#else // __BASE_UTILITY_USE_STD_COPY__

#ifdef __BASE_UTILITY_USE_MEMCPY__

    std::memcpy(destination.data() + Destination_Start,
                source.data() + Source_Start, Copy_Size * sizeof(T));

#else // __BASE_UTILITY_USE_MEMCPY__

    std::copy(source.begin() + Source_Start,
              source.begin() + Source_Start + Copy_Size,
              destination.begin() + Destination_Start);

#endif // __BASE_UTILITY_USE_MEMCPY__

#endif // __BASE_UTILITY_USE_STD_COPY__
  }
};

template <typename T, std::size_t Source_Size, std::size_t Destination_Size,
          std::size_t Source_Start, std::size_t Destination_Start>
struct CheckZeroArrayCopyForStdCopy<T, Source_Size, Destination_Size,
                                    Source_Start, Destination_Start, 0> {
  static void compute(const std::array<T, Source_Size> &source,
                      std::array<T, Destination_Size> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

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

#ifdef __BASE_UTILITY_USE_STD_COPY__

  CheckZeroArrayCopyForStdCopy<T, Source_Size, Destination_Size, Source_Start,
                               Destination_Start,
                               Copy_Size>::compute(source, destination);

#else // __BASE_UTILITY_USE_STD_COPY__

#ifdef __BASE_UTILITY_USE_MEMCPY__

  CheckZeroArrayCopyForStdCopy<T, Source_Size, Destination_Size, Source_Start,
                               Destination_Start,
                               Copy_Size>::compute(source, destination);

#else // __BASE_UTILITY_USE_MEMCPY__

  Base::Utility::COMPILED_ARRAY_COPY_PART<T, Source_Start, Copy_Size,
                                          Destination_Start, Source_Size,
                                          Destination_Size>(source,
                                                            destination);

#endif // __BASE_UTILITY_USE_MEMCPY__

#endif // __BASE_UTILITY_USE_STD_COPY__
}

/* Sort vector */
template <typename T>
inline int partition_for_sort(std::vector<T> &input_vector, int left,
                              int right) {
  T pivot = input_vector[right];
  int i = left - 1;
  for (int j = left; j < right; ++j) {
    if (input_vector[j] < pivot) {
      ++i;
      Base::Utility::swap_value(input_vector[i], input_vector[j]);
    }
  }
  Base::Utility::swap_value(input_vector[i + 1], input_vector[right]);
  return i + 1;
}

template <typename T>
inline void quick_sort(std::vector<T> &input_vector, int left, int right) {
  if (left < right) {
    int pivotIndex = partition_for_sort(input_vector, left, right);
    quick_sort(input_vector, left, pivotIndex - 1);
    quick_sort(input_vector, pivotIndex + 1, right);
  }
}

template <typename T> inline void sort(std::vector<T> &input_vector) {
  quick_sort(input_vector, 0, static_cast<int>(input_vector.size() - 1));
}

/* Sort array */
template <typename T, std::size_t N>
inline int partition_for_sort(std::array<T, N> &input_array, int left,
                              int right) {
  T pivot = input_array[right];
  int i = left - 1;
  for (int j = left; j < right; ++j) {
    if (input_array[j] < pivot) {
      ++i;
      Base::Utility::swap_value(input_array[i], input_array[j]);
    }
  }
  Base::Utility::swap_value(input_array[i + 1], input_array[right]);
  return i + 1;
}

template <typename T, std::size_t N>
inline void quick_sort(std::array<T, N> &input_array, int left, int right) {
  if (left < right) {
    int pivotIndex = partition_for_sort(input_array, left, right);
    quick_sort(input_array, left, pivotIndex - 1);
    quick_sort(input_array, pivotIndex + 1, right);
  }
}

template <typename T, std::size_t N>
inline void sort(std::array<T, N> &input_array) {
  quick_sort(input_array, 0, static_cast<int>(input_array.size() - 1));
}

} // namespace Utility
} // namespace Base

#endif // __BASE_UTILITY_HPP__
