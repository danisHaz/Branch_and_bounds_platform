#include <cassert>
#include <vector>
#include <iostream>

#include <Dense>

#include "operations.hpp"
#include "type_traits.hpp"

template < typename T1, typename T2, typename R >
struct testcase {
    T1 arg1; T2 arg2; R expected;

    testcase(T1 const& arg1, T2 const& arg2, R const& expected): arg1 { arg1 }, arg2 { arg2 }, expected { expected } {}
};

template < typename Args, babp::core::structural::OperationType operation >
void check_testcase(std::vector<Args> const& cases) {
    for (auto const& tc: cases) {
        auto result = babp::core::compute<decltype(tc.arg1), decltype(tc.arg2), operation>(tc.arg1, tc.arg2);
        auto isSame = babp::core::isSameTypes<decltype(result), decltype(tc.expected)>();
        assert(isSame);
        if (babp::core::isIntegralType<decltype(result)>() && babp::core::isIntegralType<decltype(tc.expected)>()) {
            assert(result == tc.expected);
        }
    }
}

void test_compute_integral() {
    using namespace babp::core;
    using int_test_t = testcase<int, int, int>;

    std::vector<int_test_t> cases = { { 1, 2, 3 }, { 10, 10, 20 }, { -123, 345, 222 } };
    check_testcase<int_test_t, structural::OperationType::PLUS>(cases);

    cases = { { 1, 5, 5 }, { 20, 40, 800 }, { -123, -10, 1230 } };
    check_testcase<int_test_t, structural::OperationType::MULT>(cases);

    cases = { { 1, 5, 5 }, { 20, 40, 800 }, { -123, -10, 1230 } };
    check_testcase<int_test_t, structural::OperationType::SCALAR_DIVIDER>(cases);
}

template < typename Arg_t >
babp::core::Vector_t createVector(std::vector<Arg_t> const& args) {
    babp::core::Vector_t vec;
    vec.resize(Eigen::NoChange, args.size());
    for (std::size_t i = 0; i < args.size(); i++) {
        vec.row(0).col(i) << args[i];
    }

    return vec;
}

void test_compute_vector_integral() {
    using namespace babp::core;
    using test_t = testcase<int, Vector_t, Vector_t>;

    std::vector<test_t> cases = { { 1, createVector<int>({ 1, 2 }), createVector<int>({ 1, 2 }) }, { 10, createVector<int>({ 1, 2 }), createVector<int>({ 10, 20 }) } };
    check_testcase<test_t, structural::OperationType::MULT>(cases); 
}

int main() {
    
    test_compute_integral();
    test_compute_vector_integral();

    return 0;
}