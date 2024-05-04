#include <queue>
#include <iostream>
#include <ctype.h>
#include <thread>

#include "backend.hpp"
#include "conjugate_coord_descent.hpp"
#include "operations.hpp"
#include "index_bound_set.hpp"
#include "defines.hpp"

babp::core::Vector_t babp::core::Backend::solve(std::ostream &output) const {
    Vector_t currentResult;

    std::queue<TreeNodeArguments> nodes;
    nodes.push(TreeNodeArguments { __DBL_MAX__, Indices { dimensionSize, 0 }, Indices { dimensionSize, 0 }, this->indicesGenerator });

    ConjugateCoordDescent descent { dimensionSize, boundsSize };
    std::int64_t nodesCount = 0;

    while (!nodes.empty()) {
        nodesCount++;
        auto node = nodes.front(); nodes.pop();

        PRINT(node.toString());

        auto const& lowerBound = node.lowerBound;
        auto const& upperBound = node.upperBound;
        auto generator = std::move(node.generator);
        auto fRec = node.fRec;

        // side effect function to recalculate all properties before node pushing
        recalculateBaseMatrices(lowerBound, upperBound);

        auto meme = descent.solveAlternative(calculateDerivativeFunction);

        PRINT(meme);

        Vector_t x = calculateOriginalSolution(
            meme
        );

        PRINT(x);

        auto fLow = lowerBoundOfNode.compute(lowerBound, x);

        PRINT(fLow);

        if (fLow > fRec) {
            continue;
        }

        x = coerceIn(x);

        if (!x.hasNaN()) {
            fRec = fLow;
            currentResult = x;
            PRINT(calculateCostFunction(currentResult));
            PRINT(lowerBound.toString()); PRINT(upperBound.toString());
        }

        int nextIndex = generator.next();

        PRINT(nextIndex);

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

    output << "current result: " << currentResult << "\n";
    output << "nodes count: " << nodesCount << "\n";

    return currentResult;
}

babp::core::Vector_col_t babp::core::BackendAsync::solve(
    babp::ArgumentsHolderAsync holder,
    int const threadCount,
    std::ostream &output
) {
    Vector_col_t currentResult;

    auto root = TreeNodeArguments { __DBL_MAX__, Indices { dimensionSize, 0 }, Indices { dimensionSize, 0 }, this->indicesGenerator };

    ConjugateCoordDescent descent { dimensionSize, boundsSize };
    BabpThreadPool pool { threadCount };

    PRINT("before pool start");

    pool.start();

    PRINT("pool started");

    pool.launchJob(
        [&]() {
            _solve(std::move(root), std::move(holder), descent, pool);
        }
    );

    PRINT("before pool end");
    pool.stop();

    return solution;
}


void babp::core::BackendAsync::_solve(
    TreeNodeArguments node,
    babp::ArgumentsHolderAsync holder,
    ConjugateCoordDescent const& descent,
    BabpThreadPool &threadPool
) {
    PRINT(node.toString());

    auto const& lowerBound = node.lowerBound;
    auto const& upperBound = node.upperBound;
    auto generator = std::move(node.generator);
    auto fRec = node.fRec;

    // side effect function to recalculate all properties
    recalculateBaseMatrices(
        lowerBound,
        upperBound,
        holder.A,
        holder.b,
        holder.d,
        holder.Q,
        holder.alpha,
        holder.beta,
        holder.C,
        holder.p
    );

    auto meme = descent.solveAlternative([&holder, this](Vector_t const& x) {
        return calculateDerivativeFunction(x, holder.C, holder.p);
    });

    PRINT(meme);

    Vector_t x = calculateOriginalSolution(meme, holder.A, holder.d, holder.Q);

    PRINT(x);

    auto fLow = lowerBoundOfNode.compute(lowerBound, x);

    PRINT(fLow);

    if (fLow > fRec || x.hasNaN()) {
        return;
    }

    x = coerceIn(x, holder.alpha, holder.beta);
    long double solutionCost = cost(x, holder.d, holder.Q);

    {
        std::unique_lock<std::mutex> lock(solutionMutex);
        if (solutionCost < result) {
            solution = x;
        }
    }

    fRec = fLow;
    PRINT(lowerBound.toString()); PRINT(upperBound.toString());

    int nextIndex = generator.next();

    PRINT(nextIndex);

    // push with updated lower
    if (nextIndex == -1) {
        return;
    }

    threadPool.launchJob(
        [fRec, lowerBound, upperBound, nextIndex, generator, holder, descent, &threadPool, this]() {
            _solve(
                TreeNodeArguments { fRec, lowerBound.setIndex(nextIndex), upperBound, generator },
                holder,
                descent,
                threadPool
            );
        }
    );

    threadPool.launchJob(
        [fRec, lowerBound, upperBound, nextIndex, generator, holder, descent, &threadPool, this]() {
            _solve(
                TreeNodeArguments { fRec, lowerBound, upperBound.setIndex(nextIndex), generator },
                holder,
                descent,
                threadPool
            );
        }
    );
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

babp::TaskHolderAsync::TaskHolderAsync(
    int dimensionSize,
    int boundsSize,
    std::function<
        core::Vector_col_t(
            core::Vector_col_t const&,
            core::Matrix_t const&,
            core::Vector_col_t const&
        )
    > calculateDerivativeFunction,
    std::function<
        core::Vector_col_t(
            core::Vector_col_t const&,
            core::Matrix_t const&,
            core::Vector_col_t const&,
            core::Matrix_t const&
        )
    > calculateOriginalSolution,
    std::function<
        void(
            core::Indices const&, // lowerBound
            core::Indices const&, // upperBound
            core::Matrix_t &, // A
            core::Vector_col_t &, // b
            core::Vector_col_t const&, // d
            core::Matrix_t const&, // Q
            core::Vector_col_t const&, // alpha
            core::Vector_col_t const&, // beta
            core::Matrix_t &, // C
            core::Vector_col_t & // p
        )
    > recalculateBaseMatrices,
    std::function<
        core::Vector_col_t(
            core::Vector_col_t,
            core::Vector_col_t const&,
            core::Vector_col_t const&
        )
    > coerceIn,
    std::function<
        long double(
            core::Vector_col_t const&,
            core::Vector_col_t const&,
            core::Matrix_t const&
        )
    > cost,
    core::structural::IndicesGenerator indicesGenerator,
    core::LowerBoundOfNode lowerBoundOfNode
): dimensionSize { dimensionSize },
   boundsSize { boundsSize },
   calculateDerivativeFunction { std::move(calculateDerivativeFunction) },
   calculateOriginalSolution { std::move(calculateOriginalSolution) },
   recalculateBaseMatrices { std::move(recalculateBaseMatrices) },
   coerceIn { std::move(coerceIn) },
   cost { std::move(cost) },
   indicesGenerator { std::move(indicesGenerator) },
   lowerBoundOfNode { std::move(lowerBoundOfNode) }
{}

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


babp::core::BackendAsync::BackendAsync(
    babp::TaskHolderAsync &&taskHolder
): dimensionSize { taskHolder.dimensionSize },
   boundsSize { taskHolder.boundsSize },
   calculateDerivativeFunction { std::move(taskHolder.calculateDerivativeFunction) },
   calculateOriginalSolution { std::move(taskHolder.calculateOriginalSolution) },
   recalculateBaseMatrices { std::move(taskHolder.recalculateBaseMatrices) },
   coerceIn { std::move(taskHolder.coerceIn) },
   cost { std::move(taskHolder.cost) },
   indicesGenerator { std::move(taskHolder.indicesGenerator) },
   lowerBoundOfNode { std::move(taskHolder.lowerBoundOfNode) } {}

babp::core::BackendAsync::BackendAsync(
    babp::TaskHolderAsync const& taskHolder
): dimensionSize { taskHolder.dimensionSize },
   boundsSize { taskHolder.boundsSize },
   calculateDerivativeFunction { taskHolder.calculateDerivativeFunction },
   calculateOriginalSolution { taskHolder.calculateOriginalSolution },
   recalculateBaseMatrices { taskHolder.recalculateBaseMatrices },
   coerceIn { taskHolder.coerceIn },
   cost { taskHolder.cost },
   indicesGenerator { taskHolder.indicesGenerator },
   lowerBoundOfNode { taskHolder.lowerBoundOfNode } {}

babp::ArgumentsHolderAsync::ArgumentsHolderAsync(
    core::Matrix_t const& A,
    core::Vector_col_t const& b,
    core::Vector_col_t const& d,
    core::Matrix_t const& Q,
    core::Vector_col_t const& alpha,
    core::Vector_col_t const& beta,
    core::Matrix_t const& C,
    core::Vector_col_t const& p
): A { A },
   b { b },
   d { d },
   Q { Q },
   alpha { alpha },
   beta { beta },
   C { C },
   p { p } {}