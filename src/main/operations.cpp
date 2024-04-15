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