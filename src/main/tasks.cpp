#include <iostream>
#include <cmath>
#include <functional>

#include "Dense"

#include "tasks.hpp"

std::function<int(std::vector<int> const&, int)> indexGenStraight() {
    return [](std::vector<int> const& params, int index) {
        return index;
    };
}

void solve(
    int dimensionSize,
    int boundsSize,
    Eigen::MatrixXd A,
    Eigen::VectorXd b,
    Eigen::VectorXd d,
    Eigen::MatrixXd Q,
    Eigen::VectorXd alpha,
    Eigen::VectorXd beta
) {

    std::cout << alpha << "\n" << beta << "\n";

    Eigen::MatrixXd C = (-A * Q.inverse() * A.transpose()).eval();
    Eigen::VectorXd p = (-(b + A * Q.inverse() * d)).eval();

    // std::cout << A << " C\n";

    auto derivativeFunc = [&C, &p](Eigen::VectorXd y) {
        return (C * y + p).eval();
    };

    auto originalSolutionFunc = [&Q, &d, &A](Eigen::VectorXd y) {
        return (-Q.inverse() * (d + A.transpose() * y)).eval();
    };

    auto costFunc = [&Q, &d](Eigen::VectorXd x) {
        return (0.5 * (Q * x).dot(x) + d.dot(x));
    };

    auto assistiveTaskCostFunc = [&C, &p](Eigen::VectorXd y) {
        return (0.5 * (C * y).dot(y) + p.dot(y));
    };

    auto conjugatePartialDerivativeFunc = [&C, &p](Eigen::VectorXd y, int t) {
        return C.coeff(t, t) * y[t] + 0.5 * C.row(t).dot(y) + p[t];
    };

    auto alphaParamFunc = [&C](Eigen::VectorXd e, Eigen::VectorXd t) {
        return (e.dot(C * t)) / (t.dot(C * t));
    };

    auto coerceIn = [&alpha, &beta](Eigen::VectorXd x) {
        for (int i = 0; i < x.size(); i++) {
            x[i] = std::max(x[i], alpha[i]);
            x[i] = std::min(x[i], beta[i]);
        }
        return x;
    };

    auto recalculateParams = [&](babp::core::Indices const& lowerBound, babp::core::Indices const& upperBound) {
        // std::cout << "recalc begin\n";
        std::cout << A << " A\n";
        b.fill(0);
        A.fill(0);
        lowerBound.iterateOver([&](int step){
            A(step, step) = -1;
            b[step] = -alpha[step];
        });

        upperBound.iterateOver([&](int step) {
            A(step, step) = 1;
            b[step] = beta[step];
        });

        C = (-A * Q.inverse() * A.transpose()).eval();
        p = (-(b + A * Q.inverse() * d)).eval();
        // std::cout << "recalc end\n";
    };

    auto isInProvidedBounds = [&A, &alpha, &beta](Eigen::VectorXd const& x) {
        for (int ind = 0; ind < x.size(); ind++) {
            if (A.coeff(ind, ind) == 0 || (alpha[ind] <= x[ind] && x[ind] <= beta[ind])) {
                continue;
            }

            return false;
        }

        return true;
    };

    babp::core::structural::IndicesGenerator generator {
        boundsSize, indexGenStraight()
    };

    babp::core::LowerBoundOfNode lowerBoundOfNode {
        Q, b, alpha, beta
    };

    babp::TaskHolder holder {
        dimensionSize, boundsSize,
        std::move(derivativeFunc),
        std::move(alphaParamFunc),
        std::move(originalSolutionFunc),
        std::move(recalculateParams),
        costFunc,
        std::move(assistiveTaskCostFunc),
        std::move(conjugatePartialDerivativeFunc),
        std::move(coerceIn),
        std::move(generator),
        std::move(lowerBoundOfNode),
        std::move(isInProvidedBounds)
    };

    babp::core::Backend backend { std::move(holder) };

    auto solution = backend.solve();

    // Eigen::VectorXd kek;
    // kek << alpha[0], alpha[1], alpha[2];
    // std::cout << costFunc(kek) << "\n";

    std::cout << costFunc(solution) << '\n';
}

void task1(int dimensionSize, int boundsSize) {

    Eigen::MatrixXd Q { Eigen::MatrixXd::Zero(dimensionSize, dimensionSize) };
    Eigen::VectorXd alpha; alpha.resize(dimensionSize);
    Eigen::VectorXd beta; beta.resize(dimensionSize);

    for (int index = 0; index < dimensionSize; index++) {
        alpha[index] = -1 - (index + 1);
        beta[index] = 1 + 5 * (index + 1);
    }

    for (int index = 0; index < dimensionSize; index++) {
        Q(index, index) = -(dimensionSize - 1 - 0.1 * (index + 1));
    }

    Eigen::VectorXd d { Eigen::VectorXd::Zero(dimensionSize) };

    Eigen::VectorXd b = Eigen::VectorXd::Zero(boundsSize);
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(boundsSize, dimensionSize);

    solve(dimensionSize, boundsSize, std::move(A), std::move(b), std::move(d), std::move(Q), std::move(alpha), std::move(beta));
}