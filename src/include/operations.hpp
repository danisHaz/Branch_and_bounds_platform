#pragma once

#include <cassert>

#include "algorithm.hpp"
#include "type_traits.hpp"
#include "utils.hpp"
#include "eigen_ext.hpp"

namespace babp {
namespace core {

    template < typename LeftArg_t, typename RightArg_t, structural::OperationType operation>
    constexpr auto compute(LeftArg_t const& leftArg, RightArg_t const& rightArg) {
        using Oper_t = structural::OperationType;
        if constexpr (operation == Oper_t::PLUS) {
            warningIfAny<
                isIntegralType<LeftArg_t>() && isMatrixType<RightArg_t>(),
                isIntegralType<RightArg_t>() && isMatrixType<LeftArg_t>()
            >("operations: Both computing arguments should be either literal or matrix.");
            return leftArg + rightArg;
        }

        if constexpr (operation == Oper_t::MINUS) {
            assert(!(isIntegralType<LeftArg_t>() && isMatrixType<RightArg_t>()));
            warningIfAny<
                isIntegralType<RightArg_t>() && isMatrixType<LeftArg_t>()
            >("operations: Both computing arguments should be either literal or matrix.");
            return leftArg - rightArg;
        }

        if constexpr (operation == Oper_t::MULT) {
            if constexpr (isMatrixType<LeftArg_t>() && isMatrixType<RightArg_t>()) {
                assert(canBeMultiplied(leftArg, rightArg));
            }
            return leftArg * rightArg;
        }

        if constexpr (operation == Oper_t::DIV) {
            assert(isIntegralType<LeftArg_t>() && isIntegralType<RightArg_t>());
            assert(rightArg != 0);
            return leftArg / rightArg;
        }

        if constexpr (operation == Oper_t::SCALAR_DIVIDER) {
            // constexpr bool isSame = isSameTypes<LeftArg_t, RightArg_t>();
            // assert(isSame);
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

} // namespace core
} // namespace babp