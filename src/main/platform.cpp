#include <iostream>
#include <cmath>

#include <Dense>

#include "gradient_descent.hpp"
#include "conjugate_coord_descent.hpp"

int main() {

    using namespace babp::core;

    int n = 3, m = 3;

    Eigen::Matrix3d Q;
    Q << -2.9,    0,    0,
            0, -2.8,    0,
            0,    0, -2.7;

    auto d = Eigen::Vector3d::Zero();

    Eigen::Vector3d alpha, beta;
    alpha << -2, -3, -4; beta << 6, 11, 16;

    Eigen::Vector3d b;
    b << -alpha[0], beta[1], 0;
    Eigen::Matrix3d A;
    A << 1,  0, 0,
         0, -1, 0,
         0,  0, 0;

    Eigen::Matrix C = (-A * Q.inverse() * A.transpose()).eval();
    Eigen::Vector3d p = (-(b + A * Q.inverse() * d)).eval();
    
    auto derivativeFunc = [&C, &p](Eigen::Vector3d y) {
        return (C * y + p).eval();
    };

    auto originalSolutionFunc = [&Q, &d, &A](Eigen::Vector3d y) {
        return (-Q.inverse() * (d + A.transpose() * y)).eval();
    };

    auto costFunc = [&Q, &d](Eigen::Vector3d x) {
        return (0.5 * (Q * x).dot(x) + d.dot(x));
    };

    auto alphaParamFunc = [&C](Eigen::Vector3d e, Eigen::Vector3d t) {
        return (e.dot(C * t)) / (t.dot(C * t));
    };

    ConjugateCoordDescent descent { 
        3, 3, std::move(derivativeFunc), std::move(alphaParamFunc)
    };

    std::cout << descent.solve();

    return 0;
}