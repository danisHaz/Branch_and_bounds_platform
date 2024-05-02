#pragma once

#include "types.hpp"
#include "index_bound_set.hpp"

#include <utility>

namespace babp {
namespace core {

    class ConjugateCoordDescent {

        int n;
        int boundsSize;

        public:
        ConjugateCoordDescent(
            int spaceSize,
            int boundsSize
        );

        Vector_t solve(
            std::function<double(Vector_t const&, int const)> const& calculatePartialDerivative,
            std::function<Vector_t(Vector_t const&)> const& calculateDerivativeFunction,
            std::function<double(Vector_t const&, Vector_t const&)> const& calculateAlphaParam
        ) const;

        Vector_t solveAlternative(
            std::function<Vector_t(Vector_t const&)> const& calculateDerivativeFunction
        ) const;
    };

} // namespace core
} // namespace babp