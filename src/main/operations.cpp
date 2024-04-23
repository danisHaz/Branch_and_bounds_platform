#include <cmath>

#include "operations.hpp"
#include "type_traits.hpp"

using namespace babp::core;

Var_t compute(Var_t const& leftArg, Var_t const& rightArg, structural::OperationType const operation) {
    using Oper_t = structural::OperationType;

    Var_t result;

    std::visit([&operation, &result] (auto &&left, auto &&right) {
        if (!left.has_value() || !right.has_value()) {
            return;
        }

        auto leftArg = left.value();
        auto rightArg = right.value();

        using Left_t = std::decay_t<decltype(leftArg)>;
        using Right_t = std::decay_t<decltype(rightArg)>;

        constexpr bool bothIntegral = (isIntegralType<Left_t>() && isIntegralType<Right_t>());
        constexpr bool bothMatrix = (isMatrixType<Left_t>() && isMatrixType<Right_t>());
        constexpr bool bothVectors = (isVectorType<Left_t>() && isVectorType<Right_t>());

        switch (operation) {
            case Oper_t::PLUS: {
                if constexpr (bothIntegral) {
                    auto localRes = leftArg + rightArg;
                    result = std::optional<decltype(localRes)>(localRes);
                } else if constexpr (bothMatrix) {
                    auto localRes = (leftArg + rightArg).eval();
                    result = std::optional<decltype(localRes)>(localRes);
                }
                throw std::logic_error("operations: computing operation is utility or not supported.");
                break;
            }

            case Oper_t::MINUS: {
                if constexpr (bothIntegral) {
                    auto localRes = leftArg - rightArg;
                    result = std::optional<decltype(localRes)>(localRes);
                } else if constexpr (bothMatrix) {
                    auto localRes = (leftArg - rightArg).eval();
                    result = std::optional<decltype(localRes)>(localRes);
                }
                throw std::logic_error("operations: computing operation is utility or not supported.");
                break;
            }

            case Oper_t::MULT: {
                if constexpr (bothMatrix) {
                    assert(canBeMultiplied(leftArg, rightArg));
                }
                if constexpr (bothIntegral) {
                    auto localRes = leftArg * rightArg;
                    result = std::optional<decltype(localRes)>(localRes);
                } else {
                    auto mult = (leftArg * rightArg).eval();
                    auto localRes = babp::core::matrix_static_cast<decltype(mult)>(mult);
                    result = std::optional<decltype(localRes)>(localRes);
                }
                break;
            }

            case Oper_t::DIV: {
                if constexpr (bothIntegral) {
                    auto localRes = leftArg / rightArg;
                    result = std::optional<decltype(localRes)>(localRes);
                } else {
                    throw std::logic_error("operations: computing operation is utility or not supported.");
                }
                break;
            }

            case Oper_t::SCALAR_DIVIDER: {
                if constexpr (bothIntegral) {
                    warning(
                        "operations: Both computing arguments should not be of same type in scalar multiplication."
                    );
                    auto localRes = leftArg * rightArg;
                    result = std::optional<decltype(localRes)>(localRes);
                } else if constexpr (bothVectors) {
                    auto localRes = leftArg.dot(rightArg);
                    result = std::optional<decltype(localRes)>(localRes);
                } else {
                    throw std::logic_error("operations: computing operation is utility or not supported.");
                }
                break;
            }

            default: {
                throw std::logic_error("operations: computing operation is utility or not supported.");
            }
        }
    }, leftArg, rightArg);

    return result;
}

babp::core::LowerBoundOfNode::LowerBoundOfNode(
    Matrix_t Q,
    Vector_t b,
    Vector_t alpha,
    Vector_t beta
): Q { std::move(Q) }, b { std::move(b) }, alpha { std::move(alpha) }, beta { std::move(beta) } {}

double babp::core::LowerBoundOfNode::compute(babp::core::Indices const& indices, babp::core::Vector_t const& x) const {
    double lowerBound = 0;
    indices.iterateOver([&indices, &x, &lowerBound, this](int i) {
        indices.iterateOver([&i, &x, &lowerBound, this](int j) {
            lowerBound += Q.coeff(i, j) * x[i] * x[j];
        });

        lowerBound += b[i] * x[i];
        
        indices.iterateOverNonExisting([&i, &x, &lowerBound, this](int t) {
            lowerBound += std::min(Q.coeff(t, i) * x[i] * alpha[i], Q.coeff(t, i) * x[i] * beta[i]);
        });
    });

    indices.iterateOverNonExisting([&x, &lowerBound, this](int t) {
        lowerBound += std::min(Q.coeff(t, t) * alpha[t] * alpha[t], Q.coeff(t, t) * beta[t] * beta[t]);
        lowerBound += std::min(b[t] * alpha[t], b[t] * beta[t]);
    });

    return lowerBound;
}