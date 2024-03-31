#include "utils.hpp"
#include "operations.hpp"
#include "type_traits.hpp"
#include "algorithm.hpp"

#include <Dense>
#include <iostream>

int main() {
    Eigen::Matrix2d m;
    Eigen::Matrix2d p;
    m << 1, 2, 3, 4;
    p << 1, 2, 3, 4;

    using namespace babp::core;

    auto v = compute<decltype(m), decltype(p), structural::OperationType::PLUS>(m, p);

    std::cout << v;
    return 0;
}