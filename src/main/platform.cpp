#include "utils.hpp"
#include "operations.hpp"
#include "type_traits.hpp"
#include "algorithm.hpp"

#include <Dense>
#include <iostream>

int main() {
    Eigen::Matrix<int, 1, Eigen::Dynamic> a;
    a.resize(Eigen::NoChange, 3);
    a << 1, 2, 3;
    Eigen::Matrix<int, 1, 3> res(5 * a);
    std::cout << typeid(res).name();
    return 0;
}