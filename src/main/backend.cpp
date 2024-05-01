#include <queue>
#include <iostream>
#include <ctype.h>

#include "backend.hpp"
#include "conjugate_coord_descent.hpp"
#include "operations.hpp"
#include "index_bound_set.hpp"

babp::core::Vector_t babp::core::Backend::solve() {
    Vector_t currentResult;

    std::queue<TreeNodeArguments> nodes;
    nodes.push(TreeNodeArguments { __DBL_MAX__, Indices { dimensionSize, 0 }, Indices { dimensionSize, 0 }, this->indicesGenerator });

    ConjugateCoordDescent descent { dimensionSize, boundsSize };
    std::int64_t nodesCount = 0;

    while (!nodes.empty()) {
        nodesCount++;
        auto node = nodes.front(); nodes.pop();

        std::cout << node.toString();

        auto const& lowerBound = node.lowerBound;
        auto const& upperBound = node.upperBound;
        auto generator = std::move(node.generator);
        auto fRec = node.fRec;

        // side effect function to recalculate all properties before node pushing
        recalculateBaseMatrices(lowerBound, upperBound);

        auto meme = descent.solveAlternative(conjugatePartialDerivativeFunc, calculateDerivativeFunction, assistiveTaskCostFunc, calculateAlphaParam);

        std::cout << meme << " yyy\n";

        Vector_t x = calculateOriginalSolution(
            meme
        );

        std::cout << x << " xxx\n";

        auto fLow = lowerBoundOfNode.compute(lowerBound, x);

        std::cout << fLow << " flow\n";

        if (fLow > fRec) {
            continue;
        }

        x = coerceIn(x);

        if (!x.hasNaN()) {
            fRec = fLow;
            currentResult = x;
            std::cout << calculateCostFunction(currentResult) << " curres\n";
            std::cout << lowerBound.toString() << " - lower, " << upperBound.toString() << " - upper\n";
        }

        int nextIndex = generator.next();

        std::cout << nextIndex << " next index\n";

        // push with updated lower
        if (nextIndex == -1) {
            continue;
        }
        nodes.push(
            TreeNodeArguments { fRec, lowerBound.setIndex(nextIndex), upperBound, generator }
        );

        // push with updated upper
        nodes.push(
            TreeNodeArguments { fRec, lowerBound, upperBound.setIndex(nextIndex), std::move(generator) }
        );
    }

    std::cout << "current result: " << currentResult << "\n";

    std::cout << "nodes count: " << nodesCount << "\n";

    return currentResult;
}

babp::TaskHolder::TaskHolder(
    int dimensionSize,
    int boundsSize,
    std::function<core::Vector_t(core::Vector_t const&)> calculateDerivativeFunction,
    std::function<double(core::Vector_t const&, core::Vector_t const&)> calculateAlphaParam,
    std::function<core::Vector_t(core::Vector_t const&)> calculateOriginalSolution,
    std::function<void(core::Indices const&, core::Indices const&)> recalculateBaseMatrices,
    std::function<double(core::Vector_t const&)> calculateCostFunction,
    std::function<double(core::Vector_t const&)> assistiveTaskCostFunc,
    std::function<double(core::Vector_t const&, int const)> conjugatePartialDerivativeFunc,
    std::function<core::Vector_t(core::Vector_t const&)> coerceIn,
    core::structural::IndicesGenerator indicesGenerator,
    core::LowerBoundOfNode lowerBoundOfNode,
    std::function<bool(core::Vector_t const&)> isInProvidedBounds
): dimensionSize { dimensionSize },
   boundsSize { boundsSize },
   calculateDerivativeFunction { std::move(calculateDerivativeFunction) },
   calculateAlphaParam { std::move(calculateAlphaParam) },
   calculateOriginalSolution { std::move(calculateOriginalSolution) },
   recalculateBaseMatrices { std::move(recalculateBaseMatrices) },
   calculateCostFunction { std::move(calculateCostFunction) },
   assistiveTaskCostFunc { std::move(assistiveTaskCostFunc) },
   conjugatePartialDerivativeFunc { std::move(conjugatePartialDerivativeFunc) },
   coerceIn { std::move(coerceIn) },
   indicesGenerator { std::move(indicesGenerator) },
   lowerBoundOfNode { std::move(lowerBoundOfNode) },
   isInProvidedBounds { std::move(isInProvidedBounds) } {}

babp::core::Backend::Backend(
    babp::TaskHolder &&taskHolder
): dimensionSize { taskHolder.dimensionSize },
   boundsSize { taskHolder.boundsSize },
   calculateDerivativeFunction { std::move(taskHolder.calculateDerivativeFunction) },
   calculateAlphaParam { std::move(taskHolder.calculateAlphaParam) },
   calculateOriginalSolution { std::move(taskHolder.calculateOriginalSolution) },
   recalculateBaseMatrices { std::move(taskHolder.recalculateBaseMatrices) },
   calculateCostFunction { std::move(taskHolder.calculateCostFunction) },
   assistiveTaskCostFunc { std::move(taskHolder.assistiveTaskCostFunc) },
   conjugatePartialDerivativeFunc { std::move(taskHolder.conjugatePartialDerivativeFunc) },
   coerceIn { std::move(taskHolder.coerceIn) },
   indicesGenerator { std::move(taskHolder.indicesGenerator) },
   lowerBoundOfNode { std::move(taskHolder.lowerBoundOfNode) },
   isInProvidedBounds { std::move(taskHolder.isInProvidedBounds) } {}

babp::core::Backend::Backend(
    babp::TaskHolder const& taskHolder
): dimensionSize { taskHolder.dimensionSize },
   boundsSize { taskHolder.boundsSize },
   calculateDerivativeFunction { taskHolder.calculateDerivativeFunction },
   calculateAlphaParam { taskHolder.calculateAlphaParam },
   calculateOriginalSolution { taskHolder.calculateOriginalSolution },
   recalculateBaseMatrices { taskHolder.recalculateBaseMatrices },
   calculateCostFunction { taskHolder.calculateCostFunction },
   assistiveTaskCostFunc { taskHolder.assistiveTaskCostFunc },
   conjugatePartialDerivativeFunc { taskHolder.conjugatePartialDerivativeFunc },
   coerceIn { taskHolder.coerceIn },
   indicesGenerator { taskHolder.indicesGenerator },
   lowerBoundOfNode { taskHolder.lowerBoundOfNode },
   isInProvidedBounds { taskHolder.isInProvidedBounds } {}
