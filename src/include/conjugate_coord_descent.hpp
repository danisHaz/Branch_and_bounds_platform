#pragma once

#include "types.hpp"
#include "index_bound_set.hpp"

#include <utility>

namespace babp {
namespace core {

    class ConjugateCoordDescent {

        std::size_t n;
        std::size_t boundsSize;
        std::function<Vector_t(Vector_t const&)> calculateDerivativeFunction;
        std::function<double(Vector_t const&, Vector_t const&)> calculateAlphaParam;

        public:
        ConjugateCoordDescent(
            std::size_t spaceSize,
            std::size_t boundsSize,
            std::function<Vector_t(Vector_t const&)> calculateDerivativeFunction,
            std::function<double(Vector_t const&, Vector_t const&)> calculateAlphaParam
        );

        Vector_t solve() const;
    };

} // namespace core
} // namespace babp