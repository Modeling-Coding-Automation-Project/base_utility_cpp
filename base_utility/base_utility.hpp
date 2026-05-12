/**
 * @file base_utility.hpp
 * @brief Provides generic utility templates for operations on std::vector and
 * std::array.
 *
 * Includes functions for copying (full and partial), swapping, sign
 * determination, zero-division avoidance, and sorting. Supports selection of
 * copy mechanism (manual, std::copy, or std::memcpy) and uses recursive
 * templates for compile-time operations.
 */
#ifndef BASE_UTILITY_HPP_
#define BASE_UTILITY_HPP_

#include "base_utility_macros.hpp"

#include <array>
#include <vector>

#ifdef BASE_UTILITY_USE_STD_COPY_
#include <algorithm>
#else // BASE_UTILITY_USE_STD_COPY_

#ifdef BASE_UTILITY_USE_MEMCPY_
#include <cstring>
#else  // BASE_UTILITY_USE_MEMCPY_
#endif // BASE_UTILITY_USE_MEMCPY_

#endif // BASE_UTILITY_USE_STD_COPY_

namespace Base {
namespace Utility {

/**
 * @brief Ensures that the input value is not too close to zero to avoid
 * division by zero errors.
 *
 * This function checks if the input value `in` is less than the specified
 * minimum division threshold `division_min`. If so, it adjusts the value to be
 * at least `division_min` (for non-negative inputs) or at most `-division_min`
 * (for negative inputs close to zero). This helps prevent division by zero or
 * very small numbers that could cause numerical instability.
 *
 * @tparam T Numeric type of the input value.
 * @param in The input value to be checked.
 * @param division_min The minimum absolute value allowed for division.
 * @return The adjusted value, guaranteed to be at least `division_min` in
 * magnitude if originally too close to zero.
 */
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

/**
 * @brief Checks if a value is near zero within a specified threshold.
 *
 * This function determines whether the input value `in` is considered "near
 * zero" by comparing it to a minimum division threshold `division_min`. It
 * returns true if `in` is greater than or equal to zero and less than
 * `division_min`, or if `in` is negative but greater than `-division_min`.
 *
 * @tparam T Numeric type of the input value.
 * @param in The value to check for near-zero condition.
 * @param division_min The minimum threshold to consider for near-zero
 * comparison.
 * @return true if `in` is within the range [-division_min, division_min), false
 * otherwise.
 */
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

/**
 * @brief Swaps the values of two variables.
 *
 * This function exchanges the values of the two input variables of the same
 * type.
 *
 * @tparam T The type of the variables to be swapped.
 * @param in_1 Reference to the first variable.
 * @param in_2 Reference to the second variable.
 */
template <typename T> inline void swap_value(T &in_1, T &in_2) {
  T temp;
  temp = in_1;
  in_1 = in_2;
  in_2 = temp;
}

/**
 * @brief Returns the sign of a value.
 *
 * This function returns 1 if the input value is greater than or equal to zero,
 * and -1 if the input value is less than zero.
 *
 * @tparam T Numeric type of the input value.
 * @param x The value whose sign is to be determined.
 * @return T 1 if x >= 0, -1 if x < 0.
 */
template <typename T> inline T sign(T x) {
  if (x >= 0) {
    return static_cast<T>(1);
  } else {
    return static_cast<T>(-1);
  }
}

/* copy vector */
namespace VectorCopy {

/**
 * @brief Recursive template struct to copy elements from a source vector to a
 * destination vector.
 *
 * This specialization of the Core struct copies the element at position (Index
 * - 1) from the source vector to the destination vector, then recursively calls
 * itself with Index decremented by 1. This enables compile-time unrolling of
 * the copy operation.
 *
 * @tparam T      The type of elements in the vectors.
 * @tparam N      The total number of elements to process.
 * @tparam Index  The current index being processed (should be > 0).
 * @param source      The source vector from which elements are copied.
 * @param destination The destination vector to which elements are copied.
 */
template <typename T, std::size_t N, std::size_t Index> struct Core {
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {

    destination[Index - 1] = source[Index - 1];
    Core<T, N, Index - 1>::compute(source, destination);
  }
};

/**
 * @brief Partial specialization of the Core struct for the case when the third
 * template parameter is 0.
 *
 * This specialization provides a static compute function that performs no
 * operation. It is typically used as a base case in recursive template
 * metaprogramming.
 *
 * @tparam T Type of the elements in the source and destination vectors.
 * @tparam N First template parameter, typically representing a dimension or
 * size.
 *
 * @param source The input vector (unused in this specialization).
 * @param destination The output vector (unused in this specialization).
 */
template <typename T, std::size_t N> struct Core<T, N, 0> {
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

/**
 * @brief Copies elements from the source vector to the destination vector using
 * the Core compute function.
 *
 * @tparam T The type of elements in the vectors.
 * @tparam N The size parameter used by the Core compute function.
 * @param source The vector to copy elements from.
 * @param destination The vector to copy elements to.
 */
template <typename T, std::size_t N>
static inline void copy(const std::vector<T> &source,
                        std::vector<T> &destination) {
  Core<T, N, N>::compute(source, destination);
}

} // namespace VectorCopy

/**
 * @brief Copies elements from the source vector to the destination vector.
 *
 * This function provides multiple implementations for copying elements from the
 * source vector to the destination vector, depending on compile-time macros:
 * - If `BASE_UTILITY_USE_STD_COPY_` is defined, uses `std::copy`.
 * - If `BASE_UTILITY_USE_MEMCPY_` is defined, uses `std::memcpy`.
 * - Otherwise, uses a custom `VectorCopy::copy` implementation.
 *
 * @tparam T The type of elements in the vectors.
 * @tparam N The number of elements to copy.
 * @param source The source vector to copy from.
 * @param destination The destination vector to copy to.
 *
 * @note The destination vector must be pre-allocated with at least N elements.
 * @warning No bounds checking is performed; ensure both vectors have at least N
 * elements.
 */
template <typename T, std::size_t N>
inline void copy(const std::vector<T> &source, std::vector<T> &destination) {

#ifdef BASE_UTILITY_USE_STD_COPY_

  std::copy(source.begin(), source.end(), destination.begin());

#else // BASE_UTILITY_USE_STD_COPY_

#ifdef BASE_UTILITY_USE_MEMCPY_

  std::memcpy(destination.data(), source.data(), N * sizeof(T));

#else // BASE_UTILITY_USE_MEMCPY_

  VectorCopy::copy<T, N>(source, destination);

#endif // BASE_UTILITY_USE_MEMCPY_

#endif // BASE_UTILITY_USE_STD_COPY_
}

/* copy vector part */
template <typename T, std::size_t Source_Start, std::size_t Destination_Start,
          std::size_t Copy_Size>
struct CheckZeroVectorCopyForStdCopy {
  /**
   * @brief Copies a range of elements from the source vector to the destination
   * vector.
   *
   * This function copies a specified range of elements from the input vector
   * `source` to the output vector `destination`. The copying method depends on
   * compile-time macros: it can use `std::copy`, `std::memcpy`, or a fallback
   * to `std::copy`.
   *
   * @tparam T The type of elements in the vectors.
   * @param source The source vector from which elements are copied.
   * @param destination The destination vector to which elements are copied.
   *
   * @note The range and positions for copying are determined by the constants
   *       `Source_Start`, `Copy_Size`, and `Destination_Start`, which must be
   * defined in the appropriate scope.
   * @note The function assumes that the destination vector has sufficient space
   * to accommodate the copied elements.
   * @note The actual copy implementation depends on the macros:
   *       - `BASE_UTILITY_USE_STD_COPY_` for `std::copy`
   *       - `BASE_UTILITY_USE_MEMCPY_` for `std::memcpy`
   */
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {

#ifdef BASE_UTILITY_USE_STD_COPY_

    std::copy(source.begin() + Source_Start,
              source.begin() + Source_Start + Copy_Size,
              destination.begin() + Destination_Start);

#else // BASE_UTILITY_USE_STD_COPY_

#ifdef BASE_UTILITY_USE_MEMCPY_

    std::memcpy(destination.data() + Destination_Start,
                source.data() + Source_Start, Copy_Size * sizeof(T));

#else // BASE_UTILITY_USE_MEMCPY_

    std::copy(source.begin() + Source_Start,
              source.begin() + Source_Start + Copy_Size,
              destination.begin() + Destination_Start);

#endif // BASE_UTILITY_USE_MEMCPY_

#endif // BASE_UTILITY_USE_STD_COPY_
  }
};

template <typename T, std::size_t Source_Start, std::size_t Destination_Start>
struct CheckZeroVectorCopyForStdCopy<T, Source_Start, Destination_Start, 0> {
  /**
   * @brief Computes a result based on the input source vector and stores it in
   * the destination vector.
   *
   * This is a static template function intended to be specialized or
   * overridden. The default implementation does nothing.
   *
   * @tparam T The type of elements in the vectors.
   * @param source The input vector containing source data.
   * @param destination The output vector to store computed results.
   */
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

namespace VectorCopyPart {

template <typename T, std::size_t Source_Start, std::size_t Destination_Start,
          std::size_t Index>
struct Core {
  /**
   * @brief Copies an element from the source vector to the destination vector
   * at specified indices, then recursively processes the next element.
   *
   * @tparam T                The type of elements in the vectors.
   * @tparam Source_Start     The starting index offset for the source vector.
   * @tparam Destination_Start The starting index offset for the destination
   * vector.
   * @tparam Index            The current index being processed (recursively
   * decremented).
   * @param source            The input vector from which elements are copied.
   * @param destination       The output vector to which elements are copied.
   *
   * @note This function assumes that the indices (Index + Source_Start - 1) and
   *       (Index + Destination_Start - 1) are valid for the respective vectors.
   *       The recursion terminates when Index reaches the base case (typically
   * specialized elsewhere).
   */
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {

    destination[Index + Destination_Start - 1] =
        source[Index + Source_Start - 1];
    Core<T, Source_Start, Destination_Start, Index - 1>::compute(source,
                                                                 destination);
  }
};

template <typename T, std::size_t Source_Start, std::size_t Destination_Start>
struct Core<T, Source_Start, Destination_Start, 0> {
  /**
   * @brief Computes a result based on the input source vector and stores it in
   * the destination vector.
   *
   * This static function is intended to be specialized or overridden to perform
   * specific computations. By default, it does nothing.
   *
   * @tparam T The type of elements in the vectors.
   * @param source The input vector containing source data.
   * @param destination The output vector where the result should be stored.
   */
  static void compute(const std::vector<T> &source,
                      std::vector<T> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

/**
 * @brief Copies a segment of elements from a source vector to a destination
 * vector.
 *
 * This function template copies a block of elements from the source vector to
 * the destination vector, starting at specified indices and for a specified
 * size. The copying operation is delegated to the Core::compute method, which
 * handles the actual copying logic.
 *
 * @tparam T                The type of elements in the vectors.
 * @tparam Source_Start     The starting index in the source vector from which
 * to begin copying.
 * @tparam Copy_Size        The number of elements to copy.
 * @tparam Destination_Start The starting index in the destination vector where
 * elements will be copied to.
 * @tparam Source_Size      The total size of the source vector (for
 * compile-time checks or optimizations).
 * @tparam Destination_Size The total size of the destination vector (for
 * compile-time checks or optimizations).
 * @param source            The source vector from which elements are copied.
 * @param destination       The destination vector to which elements are copied.
 */
template <typename T, std::size_t Source_Start, std::size_t Copy_Size,
          std::size_t Destination_Start, std::size_t Source_Size,
          std::size_t Destination_Size>
static inline void copy(const std::vector<T> &source,
                        std::vector<T> &destination) {

  Core<T, Source_Start, Destination_Start, Copy_Size>::compute(source,
                                                               destination);
}

} // namespace VectorCopyPart

/**
 * @brief Copies a specified range of elements from a source vector to a
 * destination vector.
 *
 * This templated function copies `Copy_Size` elements of type `T` from the
 * `source` vector, starting at index `Source_Start`, to the `destination`
 * vector, starting at index `Destination_Start`. The function supports
 * different copy mechanisms (e.g., std::copy, memcpy, or a custom
 * implementation) based on compile-time macros.
 *
 * @tparam T                The type of elements in the vectors.
 * @tparam Source_Start     The starting index in the source vector.
 * @tparam Copy_Size        The number of elements to copy.
 * @tparam Destination_Start The starting index in the destination vector.
 * @tparam Source_Size      The total size of the source vector.
 * @tparam Destination_Size The total size of the destination vector.
 * @param source            The source vector to copy elements from.
 * @param destination       The destination vector to copy elements to.
 *
 * @note The function performs compile-time checks to ensure valid copy ranges.
 * @note The actual copy mechanism depends on the macros:
 * BASE_UTILITY_USE_STD_COPY_ and BASE_UTILITY_USE_MEMCPY_.
 * @warning Undefined behavior may occur if the specified ranges exceed the
 * bounds of the vectors.
 */
template <typename T, std::size_t Source_Start, std::size_t Copy_Size,
          std::size_t Destination_Start, std::size_t Source_Size,
          std::size_t Destination_Size>
inline void copy(const std::vector<T> &source, std::vector<T> &destination) {

#ifdef BASE_UTILITY_USE_STD_COPY_

  CheckZeroVectorCopyForStdCopy<T, Source_Start, Destination_Start,
                                Copy_Size>::compute(source, destination);

#else // BASE_UTILITY_USE_STD_COPY_

#ifdef BASE_UTILITY_USE_MEMCPY_

  CheckZeroVectorCopyForStdCopy<T, Source_Start, Destination_Start,
                                Copy_Size>::compute(source, destination);

#else // BASE_UTILITY_USE_MEMCPY_

  VectorCopyPart::copy<T, Source_Start, Copy_Size, Destination_Start,
                       Source_Size, Destination_Size>(source, destination);

#endif // BASE_UTILITY_USE_MEMCPY_

#endif // BASE_UTILITY_USE_STD_COPY_
}

/* copy array */
namespace ArrayCopy {

template <typename T, std::size_t N, std::size_t Index> struct Core {
  /**
   * @brief Copies the element at position (Index - 1) from the source array to
   * the destination array, then recursively processes the remaining elements.
   *
   * @tparam T      The type of the elements in the arrays.
   * @tparam N      The size of the arrays.
   * @tparam Index  The current index being processed (should be > 0).
   * @param source      The input array from which elements are copied.
   * @param destination The output array to which elements are copied.
   */
  static void compute(const std::array<T, N> &source,
                      std::array<T, N> &destination) {

    destination[Index - 1] = source[Index - 1];
    Core<T, N, Index - 1>::compute(source, destination);
  }
};

template <typename T, std::size_t N> struct Core<T, N, 0> {
  /**
   * @brief Performs a computation from the source array to the destination
   * array.
   *
   * This static function is intended to be specialized or overridden to
   * implement specific computation logic. By default, it performs no operation.
   *
   * @tparam T The type of the elements in the arrays.
   * @tparam N The size of the arrays.
   * @param source The input array containing source data.
   * @param destination The output array to store the computed results.
   */
  static void compute(const std::array<T, N> &source,
                      std::array<T, N> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

/**
 * @brief Copies the contents of one std::array to another.
 *
 * This function copies all elements from the source array to the destination
 * array using the Core<T, N, N>::compute method.
 *
 * @tparam T The type of the elements in the array.
 * @tparam N The size of the array.
 * @param source The source array to copy from.
 * @param destination The destination array to copy to.
 */
template <typename T, std::size_t N>
static inline void copy(const std::array<T, N> &source,
                        std::array<T, N> &destination) {
  Core<T, N, N>::compute(source, destination);
}

} // namespace ArrayCopy

/**
 * @brief Copies the contents of one std::array to another.
 *
 * This function copies all elements from the source array to the destination
 * array. The implementation can use std::copy, std::memcpy, or a custom
 * ArrayCopy::copy depending on the preprocessor macros defined:
 * - If BASE_UTILITY_USE_STD_COPY_ is defined, uses std::copy.
 * - Else if BASE_UTILITY_USE_MEMCPY_ is defined, uses std::memcpy.
 * - Otherwise, uses ArrayCopy::copy.
 *
 * @tparam T The type of the elements in the array.
 * @tparam N The number of elements in the array.
 * @param source The source array to copy from.
 * @param destination The destination array to copy to.
 */
template <typename T, std::size_t N>
inline void copy(const std::array<T, N> &source,
                 std::array<T, N> &destination) {

#ifdef BASE_UTILITY_USE_STD_COPY_

  std::copy(source.begin(), source.end(), destination.begin());

#else // BASE_UTILITY_USE_STD_COPY_

#ifdef BASE_UTILITY_USE_MEMCPY_

  std::memcpy(destination.data(), source.data(), N * sizeof(T));

#else // BASE_UTILITY_USE_MEMCPY_

  ArrayCopy::copy<T, N>(source, destination);

#endif // BASE_UTILITY_USE_MEMCPY_

#endif // BASE_UTILITY_USE_STD_COPY_
}

/* copy array part */
template <typename T, std::size_t Source_Size, std::size_t Destination_Size,
          std::size_t Source_Start, std::size_t Destination_Start,
          std::size_t Copy_Size>
struct CheckZeroArrayCopyForStdCopy {

  /**
   * @brief Copies a range of elements from a source std::array to a destination
   * std::array.
   *
   * This static function copies a contiguous block of elements from the source
   * array, starting at index `Source_Start` and copying `Copy_Size` elements,
   * into the destination array starting at index `Destination_Start`. The
   * copying method can be selected at compile time using the macros
   * `BASE_UTILITY_USE_STD_COPY_` or `BASE_UTILITY_USE_MEMCPY_`.
   *
   * @tparam T               The type of elements in the arrays.
   * @tparam Source_Size     The size of the source array.
   * @tparam Destination_Size The size of the destination array.
   * @tparam Source_Start    The starting index in the source array.
   * @tparam Destination_Start The starting index in the destination array.
   * @tparam Copy_Size       The number of elements to copy.
   * @param source           The source array to copy from.
   * @param destination      The destination array to copy to.
   *
   * @note The function does not perform bounds checking. It is the caller's
   * responsibility to ensure that the specified ranges are valid within the
   * source and destination arrays.
   */
  static void compute(const std::array<T, Source_Size> &source,
                      std::array<T, Destination_Size> &destination) {

#ifdef BASE_UTILITY_USE_STD_COPY_

    std::copy(source.begin() + Source_Start,
              source.begin() + Source_Start + Copy_Size,
              destination.begin() + Destination_Start);

#else // BASE_UTILITY_USE_STD_COPY_

#ifdef BASE_UTILITY_USE_MEMCPY_

    std::memcpy(destination.data() + Destination_Start,
                source.data() + Source_Start, Copy_Size * sizeof(T));

#else // BASE_UTILITY_USE_MEMCPY_

    std::copy(source.begin() + Source_Start,
              source.begin() + Source_Start + Copy_Size,
              destination.begin() + Destination_Start);

#endif // BASE_UTILITY_USE_MEMCPY_

#endif // BASE_UTILITY_USE_STD_COPY_
  }
};

template <typename T, std::size_t Source_Size, std::size_t Destination_Size,
          std::size_t Source_Start, std::size_t Destination_Start>
struct CheckZeroArrayCopyForStdCopy<T, Source_Size, Destination_Size,
                                    Source_Start, Destination_Start, 0> {
  /**
   * @brief Computes a transformation from a source array to a destination
   * array.
   *
   * This static function is intended to perform a computation or transformation
   * from the input source array to the output destination array. In its current
   * implementation, it does nothing and serves as a placeholder or default
   * implementation.
   *
   * @tparam T                The type of the elements in the arrays.
   * @tparam Source_Size      The size of the source array.
   * @tparam Destination_Size The size of the destination array.
   * @param source            The input array to be used as the source.
   * @param destination       The output array to store the result.
   */
  static void compute(const std::array<T, Source_Size> &source,
                      std::array<T, Destination_Size> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

namespace ArrayCopyPart {

template <typename T, std::size_t Source_Size, std::size_t Destination_Size,
          std::size_t Source_Start, std::size_t Destination_Start,
          std::size_t Index>
struct Core {
  /**
   * @brief Copies an element from the source array to the destination array at
   * a specific index, then recursively processes the remaining elements.
   *
   * @tparam T                The type of elements in the arrays.
   * @tparam Source_Size      The size of the source array.
   * @tparam Destination_Size The size of the destination array.
   * @tparam Source_Start     The starting index in the source array (1-based).
   * @tparam Destination_Start The starting index in the destination array
   * (1-based).
   * @tparam Index            The current index being processed (1-based,
   * decremented recursively).
   * @param source            The source array from which elements are copied.
   * @param destination       The destination array to which elements are
   * copied.
   *
   * @note This function is intended to be used as part of a recursive template
   * structure. The recursion ends when Index reaches 0 (handled by a template
   * specialization).
   */
  static void compute(const std::array<T, Source_Size> &source,
                      std::array<T, Destination_Size> &destination) {

    destination[Index + Destination_Start - 1] =
        source[Index + Source_Start - 1];
    Core<T, Source_Size, Destination_Size, Source_Start, Destination_Start,
         Index - 1>::compute(source, destination);
  }
};

template <typename T, std::size_t Source_Size, std::size_t Destination_Size,
          std::size_t Source_Start, std::size_t Destination_Start>
struct Core<T, Source_Size, Destination_Size, Source_Start, Destination_Start,
            0> {
  /**
   * @brief Copies or transforms data from a source array to a destination
   * array.
   *
   * This static function is intended to perform computations or transformations
   * from the input source array to the output destination array. The default
   * implementation does nothing and is meant to be specialized or overridden
   * as needed.
   *
   * @tparam T                The type of elements in the arrays.
   * @tparam Source_Size      The size of the source array.
   * @tparam Destination_Size The size of the destination array.
   * @param source            The input array containing source data.
   * @param destination       The output array to store the computed results.
   */
  static void compute(const std::array<T, Source_Size> &source,
                      std::array<T, Destination_Size> &destination) {
    /* Do Nothing. */
    static_cast<void>(source);
    static_cast<void>(destination);
  }
};

/**
 * @brief Copies a subrange of elements from a source std::array to a
 * destination std::array.
 *
 * This function copies a specified number of elements (`Copy_Size`) starting
 * from a given index (`Source_Start`) in the source array to a specified
 * starting index (`Destination_Start`) in the destination array. The operation
 * is performed using the `Core::compute` method.
 *
 * @tparam T                The type of the elements in the arrays.
 * @tparam Source_Start     The starting index in the source array from which to
 * begin copying.
 * @tparam Copy_Size        The number of elements to copy.
 * @tparam Destination_Start The starting index in the destination array where
 * elements will be copied to.
 * @tparam Source_Size      The size of the source array.
 * @tparam Destination_Size The size of the destination array.
 * @param source            The source array from which elements are copied.
 * @param destination       The destination array to which elements are copied.
 */
template <typename T, std::size_t Source_Start, std::size_t Copy_Size,
          std::size_t Destination_Start, std::size_t Source_Size,
          std::size_t Destination_Size>
static inline void copy(const std::array<T, Source_Size> &source,
                        std::array<T, Destination_Size> &destination) {

  Core<T, Source_Size, Destination_Size, Source_Start, Destination_Start,
       Copy_Size>::compute(source, destination);
}

} // namespace ArrayCopyPart

/**
 * @brief Copies a specified portion of elements from a source std::array to a
 * destination std::array.
 *
 * This function template allows copying a contiguous block of elements from the
 * source array, starting at index Source_Start, to the destination array,
 * starting at index Destination_Start. The number of elements copied is
 * specified by Copy_Size. The sizes of the source and destination arrays are
 * given by Source_Size and Destination_Size, respectively.
 *
 * The implementation can use either std::copy, memcpy, or a custom copy
 * routine, depending on compile-time macros (BASE_UTILITY_USE_STD_COPY_ or
 * BASE_UTILITY_USE_MEMCPY_). Additional compile-time checks may be performed
 * to ensure safe copying.
 *
 * @tparam T                Type of the elements in the arrays.
 * @tparam Source_Start     Starting index in the source array.
 * @tparam Copy_Size        Number of elements to copy.
 * @tparam Destination_Start Starting index in the destination array.
 * @tparam Source_Size      Size of the source array.
 * @tparam Destination_Size Size of the destination array.
 * @param source            The source std::array to copy from.
 * @param destination       The destination std::array to copy to.
 */
template <typename T, std::size_t Source_Start, std::size_t Copy_Size,
          std::size_t Destination_Start, std::size_t Source_Size,
          std::size_t Destination_Size>
inline void copy(const std::array<T, Source_Size> &source,
                 std::array<T, Destination_Size> &destination) {

#ifdef BASE_UTILITY_USE_STD_COPY_

  CheckZeroArrayCopyForStdCopy<T, Source_Size, Destination_Size, Source_Start,
                               Destination_Start,
                               Copy_Size>::compute(source, destination);

#else // BASE_UTILITY_USE_STD_COPY_

#ifdef BASE_UTILITY_USE_MEMCPY_

  CheckZeroArrayCopyForStdCopy<T, Source_Size, Destination_Size, Source_Start,
                               Destination_Start,
                               Copy_Size>::compute(source, destination);

#else // BASE_UTILITY_USE_MEMCPY_

  ArrayCopyPart::copy<T, Source_Start, Copy_Size, Destination_Start,
                      Source_Size, Destination_Size>(source, destination);

#endif // BASE_UTILITY_USE_MEMCPY_

#endif // BASE_UTILITY_USE_STD_COPY_
}

/* Sort vector */
/**
 * @brief Partitions a vector for use in sorting algorithms (e.g., quicksort).
 *
 * Rearranges the elements in the input vector such that all elements less than
 * the pivot are moved before it, and all elements greater than or equal to the
 * pivot are moved after it. The pivot is chosen as the element at the 'right'
 * index.
 *
 * @tparam T Type of elements in the vector. Must support comparison and
 * swapping.
 * @param input_vector Reference to the vector to be partitioned.
 * @param left Index of the left boundary of the partitioning range (inclusive).
 * @param right Index of the right boundary of the partitioning range
 * (inclusive).
 * @return int The index position of the pivot after partitioning.
 */
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

/**
 * @brief Sorts a vector in place using the Quick Sort algorithm.
 *
 * This function recursively sorts the elements of the given vector between the
 * specified left and right indices using the Quick Sort algorithm. It relies on
 * a partitioning function (`partition_for_sort`) to divide the vector into
 * subarrays.
 *
 * @tparam T The type of elements in the vector.
 * @param input_vector The vector to be sorted.
 * @param left The starting index of the subarray to sort.
 * @param right The ending index of the subarray to sort.
 */
template <typename T>
inline void quick_sort(std::vector<T> &input_vector, int left, int right) {
  if (left < right) {

    int pivotIndex = partition_for_sort(input_vector, left, right);
    quick_sort(input_vector, left, pivotIndex - 1);
    quick_sort(input_vector, pivotIndex + 1, right);
  }
}

/**
 * @brief Sorts the given vector in-place using the quick sort algorithm.
 *
 * This function sorts the elements of the provided std::vector<T> in ascending
 * order. It utilizes the quick_sort function to perform the sorting operation.
 *
 * @tparam T The type of elements in the vector. Must be comparable using
 * operator<.
 * @param input_vector Reference to the vector to be sorted.
 */
template <typename T> inline void sort(std::vector<T> &input_vector) {
  quick_sort(input_vector, 0, static_cast<int>(input_vector.size() - 1));
}

/* Sort array */
/**
 * @brief Partitions the given array segment for sorting algorithms (e.g.,
 * quicksort).
 *
 * Rearranges the elements in the specified range [left, right] of the input
 * array such that all elements less than the pivot (chosen as the rightmost
 * element) are moved before it, and all elements greater than or equal to the
 * pivot are moved after it.
 *
 * @tparam T Type of the elements in the array.
 * @tparam N Size of the array.
 * @param input_array Reference to the array to be partitioned.
 * @param left Index of the left boundary of the segment to partition.
 * @param right Index of the right boundary of the segment to partition (pivot
 * element).
 * @return The index position of the pivot after partitioning.
 */
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

/**
 * @brief Sorts the elements of the given std::array in place using the Quick
 * Sort algorithm.
 *
 * This function recursively sorts the elements in the specified range [left,
 * right] of the input_array using the Quick Sort algorithm. It relies on a
 * partitioning function, partition_for_sort, to divide the array into
 * subarrays.
 *
 * @tparam T The type of elements stored in the array.
 * @tparam N The size of the array.
 * @param input_array Reference to the std::array to be sorted.
 * @param left The starting index of the range to sort.
 * @param right The ending index of the range to sort.
 */
template <typename T, std::size_t N>
inline void quick_sort(std::array<T, N> &input_array, int left, int right) {
  if (left < right) {

    int pivotIndex = partition_for_sort(input_array, left, right);
    quick_sort(input_array, left, pivotIndex - 1);
    quick_sort(input_array, pivotIndex + 1, right);
  }
}

/**
 * @brief Sorts the given std::array in-place using the quick sort algorithm.
 *
 * This function sorts the elements of the provided std::array in ascending
 * order. It utilizes the quick_sort function to perform the sorting operation.
 *
 * @tparam T The type of elements stored in the array.
 * @tparam N The number of elements in the array.
 * @param input_array Reference to the std::array to be sorted.
 */
template <typename T, std::size_t N>
inline void sort(std::array<T, N> &input_array) {
  quick_sort(input_array, 0, static_cast<int>(input_array.size() - 1));
}

} // namespace Utility
} // namespace Base

#endif // BASE_UTILITY_HPP_
