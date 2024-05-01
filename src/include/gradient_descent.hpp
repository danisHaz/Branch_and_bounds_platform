#pragma once

#include "types.hpp"

namespace babp {
namespace core {

    template < typename Argument_t >
    class GradientDescent {
        double const RATE = 0.05;
        Argument_t initialX;
        std::function<Argument_t(Argument_t const&)> calculateTarget;
        std::function<bool(Argument_t const&, Argument_t const&)> checkIfDistinct;
        int const MAX_ITER_COUNT;

        public:
        GradientDescent(
            Argument_t initialValue,
            std::function<Argument_t(Argument_t const&)> calculateTarget,
            std::function<bool(Argument_t const&, Argument_t const&)> checkIfDistinct,
            int maxIterationCount = 1000
        ): initialX { initialValue },
           calculateTarget { std::move(calculateTarget) },
           checkIfDistinct { std::move(checkIfDistinct) },
           MAX_ITER_COUNT { maxIterationCount } {}

        Argument_t solve() const {
            bool isFirstIteration = true;
            Argument_t prevX;
            Argument_t x { initialX };
            int iterStep = 0;
            while (isFirstIteration || (checkIfDistinct(prevX, x) && iterStep < MAX_ITER_COUNT)) {
                isFirstIteration = false;
                iterStep++;

                prevX = x;
                x -= RATE * calculateTarget(x);
            }

            return x;
        }

    };

} // core
} // babp