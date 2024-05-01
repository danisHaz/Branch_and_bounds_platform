#include <functional>
#include <float.h>
#include <iostream>

#include <Dense>

#include "conjugate_coord_descent.hpp"
#include "index_bound_set.hpp"
#include "gradient_descent.hpp"

babp::core::ConjugateCoordDescent::ConjugateCoordDescent(
    std::size_t spaceSize,
    std::size_t boundsSize
): n { spaceSize },
   boundsSize { boundsSize } {}

babp::core::Vector_t babp::core::ConjugateCoordDescent::solve(
    std::function<double(Vector_t const&, int const)> const& calculatePartialDerivative,
    std::function<Vector_t(Vector_t const&)> const& calculateDerivativeFunction,
    std::function<double(Vector_t const&)> const& calculcateMainFunction,
    std::function<double(Vector_t const&, Vector_t const&)> const& calculateAlphaParam
) const {
    using namespace babp::core;

    Vector_t y { Vector_t::Zero(boundsSize) };
    Vector_t d { Vector_t::Zero(boundsSize) };
    Indices indices { boundsSize, 0 }, m_set { boundsSize, (1 << (boundsSize + 1)) - 1 };

    Matrix_t identity { Matrix_t::Identity(boundsSize, boundsSize) };
    Matrix_t t { Matrix_t::Zero(boundsSize, boundsSize) };

    for (std::size_t k_iteration = 0; k_iteration <= n; k_iteration++) {
        // a-part of algorithm
        int l_index { 0 }; double minResult { DBL_MAX };

        m_set.iterateOver([this, &l_index, &minResult, &calculatePartialDerivative, &y](int step) {
            auto result = calculatePartialDerivative(y, step);
            if (minResult > result) {
                minResult = result;
                l_index = step;
            }
        });

        m_set = m_set.removeIndex(l_index);

        // b-part of algorithm
        if (k_iteration == 0) {
            d[l_index] = 1;
            t.row(l_index) = d;
            indices = indices.setIndex(l_index);
            continue;
        }

        Vector_t sum { Vector_t::Zero(boundsSize) };

        indices.iterateOver([&identity, &l_index, &calculateAlphaParam, &t, &sum](int step) {
            double alpha = calculateAlphaParam(identity.row(l_index), t.row(step));
            // std::cout << identity.row(l_index) << " " << t.row(step) << " alpha\n";
            sum += alpha * t.row(step);
        });

        // std::cout << sum << " sum\n";

        Vector_t p = identity.row(l_index) - sum;

        // c-part of algorithm
        bool isEqual = false;
        if (p == Vector_t::Zero(boundsSize)) {
            isEqual = true;
        }

        indices.iterateOver([&t, &p, &isEqual](int step) {
            if (p == t.row(step)) {
                isEqual = true;
            }
        });

        if (isEqual) {
            continue;
        }

        d = p;
        t.row(l_index) = p;

        indices = indices.setIndex(l_index);
        GradientDescent<double> descentAlgo {
            1,
            [&y, &d, &calculateDerivativeFunction](double x) {
                auto vec = calculateDerivativeFunction(y + x * d);
                // std::cout << y << " y vec\n";
                // std::cout << d << " d vec\n";
                double nextX = 0;
                for (int ind = 0; ind < d.size(); ind++) {
                    if (d[ind] == 0) {
                        continue;
                    }
                    nextX = (vec - y)[ind] / d[ind]; // ???
                    break;
                }

                // std::cout << nextX << " nextX\n";

                return nextX;
            },
            [](double prev, double cur) {
                return (abs(prev - cur) > 0.05);
            }
        };

        double lambda = std::max(descentAlgo.solve(), 0.0);

        // std::cout << lambda << " lambda\n";

        y = y + lambda * d;
    }

    return y;
}

babp::core::Vector_t babp::core::ConjugateCoordDescent::solveAlternative(
    std::function<double(Vector_t const&, int const)> const& calculatePartialDerivative,
    std::function<Vector_t(Vector_t const&)> const& calculateDerivativeFunction,
    std::function<double(Vector_t const&)> const& calculcateMainFunction,
    std::function<double(Vector_t const&, Vector_t const&)> const& calculateAlphaParam
) const {
    Vector_t prevY { boundsSize }; prevY.fill(1);
    Vector_t y { Vector_t::Zero(boundsSize) };
    double EPS = 0.005;
    int k = 0, MAX_K = 1000;
    while (k < MAX_K && (prevY - y).norm() > EPS) {
        auto antigrad = (-calculateDerivativeFunction(y)).eval();
        auto prevAntigrad = antigrad;
        auto direction = antigrad;
        auto partialSolution = y;
        for (int j = 1; j < n; j++) {
            GradientDescent<double> descentAlgo {
                1,
                [&partialSolution, &direction, &calculateDerivativeFunction](double x) {
                    auto vec = calculateDerivativeFunction(partialSolution + x * direction);
                    // std::cout << y << " y vec\n";
                    // std::cout << d << " d vec\n";
                    double nextX = 0;
                    for (int ind = 0; ind < direction.size(); ind++) {
                        if (direction[ind] == 0) {
                            continue;
                        }
                        nextX = (vec - partialSolution)[ind] / direction[ind]; // ???
                        break;
                    }

                    // std::cout << nextX << " nextX\n";

                    return nextX;
                },
                [](double prev, double cur) {
                    return (abs(prev - cur) > 0.05);
                }
            };

            double lambda = std::max(descentAlgo.solve(), 0.0);
            partialSolution = (partialSolution + lambda * direction).eval();
            auto w = std::max(0.0, (antigrad.dot(antigrad)) / (prevAntigrad.dot(prevAntigrad)));

            direction = (-calculateDerivativeFunction(partialSolution) + w * direction).eval();
        }

        prevY = y;
        y = partialSolution;
        k++;
    }

    return y;
}