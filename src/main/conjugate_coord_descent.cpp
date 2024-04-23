#include <functional>
#include <float.h>
#include <iostream>

#include <Dense>

#include "conjugate_coord_descent.hpp"
#include "index_bound_set.hpp"
#include "gradient_descent.hpp"

babp::core::ConjugateCoordDescent::ConjugateCoordDescent(
    std::size_t spaceSize,
    std::size_t boundsSize,
    std::function<Vector_t(Vector_t const&)> calculateDerivativeFunction,
    std::function<double(Vector_t const&, Vector_t const&)> calculateAlphaParam
): n { spaceSize },
   boundsSize { boundsSize },
   calculateDerivativeFunction { std::move(calculateDerivativeFunction) },
   calculateAlphaParam { std::move(calculateAlphaParam) } {}

babp::core::Vector_t babp::core::ConjugateCoordDescent::solve() const {
    using namespace babp::core;

    Vector_t y; y.resize(n); y.fill(0);
    Vector_t d; d.resize(n); d.fill(0);
    Indices indices { 1 << boundsSize, 0 }, m_set { 1 << boundsSize, (1 << (boundsSize + 1)) - 1 };

    Eigen::MatrixXd identity { Eigen::MatrixXd::Identity(boundsSize, boundsSize) };
    Eigen::MatrixXd t { boundsSize, boundsSize };

    for (std::size_t k_iteration = 0; k_iteration <= n; k_iteration++) {
        // a-part of algorithm
        int l_index { 0 }; double minResult { DBL_MAX };

        indices.iterateOver([&l_index, &minResult, this, &y](int step) {
            auto result = calculateDerivativeFunction(y);
            if (minResult > result[step]) {
                minResult = result[step];
                l_index = step;
            }
        });

        m_set = m_set.removeIndex(l_index);

        // b-part of algorithm
        if (k_iteration == 0) {
            d[l_index] = 1;
            indices = indices.setIndex(l_index);
            continue;
        }

        Vector_t sum { boundsSize }; sum.fill(0);

        indices.iterateOver([this, &identity, &l_index, &t, &sum](int step) {
            double alpha = calculateAlphaParam(identity.row(l_index), t.row(step));
            sum += alpha * t.row(step);
        });

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
            [this, &y, &d](double x) {
                auto vec = calculateDerivativeFunction(y + x * d);
                double nextX;
                for (int ind = 0; ind < d.size(); ind++) {
                    if (d[ind] == 0) {
                        continue;
                    }
                    nextX = (vec - y)[ind] / d[ind];
                    break;
                }

                return nextX;
            },
            [](double prev, double cur) {
                return (abs(prev - cur) < 0.5);
            }
        };

        double lambda = descentAlgo.solve();

        y = y + lambda * d;
    }

    return y;
}