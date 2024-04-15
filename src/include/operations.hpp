#pragma once

#include <cassert>
#include <variant>

#include "type_traits.hpp"
#include "utils.hpp"
#include "eigen_ext.hpp"

namespace babp {
namespace core {
namespace structural {

    enum class OperationType {
        PLUS, MINUS, MULT, DIV, SCALAR_START, SCALAR_DIVIDER, SCALAR_END, GROUP_START, GROUP_END
    };
} // namespace structural

    template < typename LeftArg_t, typename RightArg_t, structural::OperationType operation >
    constexpr auto compute(LeftArg_t const& leftArg, RightArg_t const& rightArg) {
        using Oper_t = structural::OperationType;
        constexpr bool isCompatibleTypes = (isIntegralType<LeftArg_t>() && isIntegralType<RightArg_t>()) ||
                (isMatrixType<RightArg_t>() && isMatrixType<LeftArg_t>());

        if constexpr (operation == Oper_t::PLUS && isCompatibleTypes) {
            return leftArg + rightArg;
        }

        if constexpr (operation == Oper_t::MINUS && isCompatibleTypes) {
            return leftArg - rightArg;
        }

        if constexpr (operation == Oper_t::MULT) {
            if constexpr (isMatrixType<LeftArg_t>() && isMatrixType<RightArg_t>()) {
                assert(canBeMultiplied(leftArg, rightArg));
            }
            return leftArg * rightArg;
        }

        if constexpr (operation == Oper_t::DIV) {
            if constexpr (isIntegralType<LeftArg_t>() && isIntegralType<RightArg_t>() && rightArg != 0) {
                return leftArg / rightArg;
            } else {
                assert(false);
            }
        }

        if constexpr (operation == Oper_t::SCALAR_DIVIDER) {
            if constexpr (isIntegralType<LeftArg_t>() && isIntegralType<RightArg_t>()) {
                warning(
                    "operations: Both computing arguments should not be of same type in scalar multiplication."
                );
                return leftArg * rightArg;
            } else {
                return leftArg.dot(rightArg);
            }
        }

        throw std::logic_error("operations: computing operation is utility or not supported.");
    }

    Var_t compute(Var_t const& leftArg, Var_t const& rightArg, structural::OperationType const operation);

} // namespace core
} // namespace babp