#include <type_traits>
#include <iostream>
#include <cmath>
#include <vector>
#include <array>
#include <stdexcept>

#include "base_utility.hpp"

#include "MCAP_tester.hpp"


using namespace Tester;

template <typename T>
void check_base_utility(void) {

    using namespace Base::Utility;

    constexpr T NEAR_LIMIT_STRICT = std::is_same<T, double>::value ? T(1.0e-6) : T(1.0e-5);

    MCAPTester<T> tester;

    T divixion_min = static_cast<T>(1.0e-10);

    /* avoid zero divide */
    std::vector<T> avoid_zero_divide_input_vector = {
        static_cast<T>(-1),
        static_cast<T>(-1.0e-10),
        static_cast<T>(-1.0e-20),
        static_cast<T>(0),
        static_cast<T>(1.0e-20),
        static_cast<T>(1.0e-10),
        static_cast<T>(1)};

    std::vector<T> avoid_zero_divide_answer_vector = {
        static_cast<T>(-1),
        static_cast<T>(-1.0e-10),
        static_cast<T>(-1.0e-20),
        static_cast<T>(1.0e-10),
        static_cast<T>(1.0e-10),
        static_cast<T>(1.0e-10),
        static_cast<T>(1) };


    for (std::size_t i = 0; i < avoid_zero_divide_input_vector.size(); i++) {
        T avoid_zero_divide_value = avoid_zero_divide(avoid_zero_divide_input_vector[i], divixion_min);
        tester.expect_near(avoid_zero_divide_value, avoid_zero_divide_answer_vector[i], NEAR_LIMIT_STRICT,
            "check avoid zero divide.");
    }

    /* near zero */
    std::vector<T> near_zero_input_vector = {
        static_cast<T>(-1),
        static_cast<T>(-1.0e-10),
        static_cast<T>(-1.0e-20),
        static_cast<T>(0),
        static_cast<T>(1.0e-20),
        static_cast<T>(1.0e-10),
        static_cast<T>(1)};

    std::vector<bool> near_zero_answer_vector = {
        false,
        false,
        true,
        true,
        true,
        false,
        false };


    for (std::size_t i = 0; i < near_zero_input_vector.size(); i++) {
        bool near_zero_value = near_zero(near_zero_input_vector[i], divixion_min);
        if (near_zero_value != near_zero_answer_vector[i]) {
            throw std::runtime_error("Test failed.");
        }
    }

    /* swap value */
    T swap_value_1 = static_cast<T>(1.0);
    T swap_value_2 = static_cast<T>(2.0);

    swap_value(swap_value_1, swap_value_2);

    tester.expect_near(swap_value_1, static_cast<T>(2.0), NEAR_LIMIT_STRICT,
        "check swap value.");

    /* sign */
    std::vector<T> sign_input_vector = {
        static_cast<T>(-1),
        static_cast<T>(-1.0e-10),
        static_cast<T>(-1.0e-20),
        static_cast<T>(0),
        static_cast<T>(1.0e-20),
        static_cast<T>(1.0e-10),
        static_cast<T>(1)};

    std::vector<T> sign_answer_vector = {
        static_cast<T>(-1),
        static_cast<T>(-1),
        static_cast<T>(-1),
        static_cast<T>(1),
        static_cast<T>(1),
        static_cast<T>(1),
        static_cast<T>(1) };

    for (std::size_t i = 0; i < sign_input_vector.size(); i++) {
        T sign_value = sign(sign_input_vector[i]);
        tester.expect_near(sign_value, sign_answer_vector[i], NEAR_LIMIT_STRICT,
            "check sign.");
    }

    /* copy vector */
    std::vector<T> copy_input_vector = {
    static_cast<T>(1),
    static_cast<T>(2),
    static_cast<T>(3),
    static_cast<T>(4),
    static_cast<T>(5),
    static_cast<T>(6),
    static_cast<T>(7) };

    std::vector<T> copy_input_destination(7);
    Base::Utility::copy<T, 7>(copy_input_vector, copy_input_destination);

    for (std::size_t i = 0; i < copy_input_vector.size(); i++) {
        tester.expect_near(copy_input_destination[i], copy_input_vector[i], NEAR_LIMIT_STRICT,
            "check copy vector.");
    }

    std::vector<T> copy_part_destination(6);
    Base::Utility::copy<T, 2, 3, 1, 7, 6>(copy_input_vector, copy_part_destination);

    std::vector<T> copy_part_vector_answer({
        static_cast<T>(0),
        static_cast<T>(3),
        static_cast<T>(4),
        static_cast<T>(5),
        static_cast<T>(0),
        static_cast<T>(0) });

    tester.expect_near(copy_part_destination, copy_part_vector_answer, NEAR_LIMIT_STRICT,
        "check copy vector part.");


    /* copy array */
    std::array<T, 7> copy_input_array = {
        static_cast<T>(1),
        static_cast<T>(2),
        static_cast<T>(3),
        static_cast<T>(4),
        static_cast<T>(5),
        static_cast<T>(6),
        static_cast<T>(7) };

    std::array<T, 7> copy_input_destination_array;
    Base::Utility::copy<T, 7>(copy_input_array, copy_input_destination_array);

    for (std::size_t i = 0; i < copy_input_array.size(); i++) {
        tester.expect_near(copy_input_destination_array[i], copy_input_array[i], NEAR_LIMIT_STRICT,
            "check copy vector.");
    }

    std::array<T, 6> copy_part_destination_array({ static_cast<T>(0) });
    Base::Utility::copy<T, 2, 3, 1, 7, 6>(copy_input_array, copy_part_destination_array);

    std::array<T, 6> copy_part_array_answer({
        static_cast<T>(0),
        static_cast<T>(3),
        static_cast<T>(4),
        static_cast<T>(5),
        static_cast<T>(0),
        static_cast<T>(0) });

    tester.expect_near(copy_part_destination_array, copy_part_array_answer, NEAR_LIMIT_STRICT,
        "check copy vector part.");


    tester.throw_error_if_test_failed();
}

int main() {

    check_base_utility<double>();

    check_base_utility<float>();


    return 0;
}
