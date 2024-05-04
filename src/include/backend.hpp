#pragma once

#include <functional>
#include <iostream>
#include <mutex>
#include <ctype.h>

#include "conjugate_coord_descent.hpp"
#include "types.hpp"
#include "index_bound_set.hpp"
#include "algorithm.hpp"
#include "thread_pool.hpp"

namespace babp {

    struct TaskHolder {
        int dimensionSize, boundsSize;
        std::function<core::Vector_t(core::Vector_t const&)> calculateDerivativeFunction;
        std::function<double(core::Vector_t const&, core::Vector_t const&)> calculateAlphaParam;
        std::function<core::Vector_t(core::Vector_t const&)> calculateOriginalSolution;
        std::function<void(core::Indices const&, core::Indices const&)> recalculateBaseMatrices;
        std::function<double(core::Vector_t const&)> calculateCostFunction;
        std::function<double(core::Vector_t const&)> assistiveTaskCostFunc;
        std::function<double(core::Vector_t const&, int const)> conjugatePartialDerivativeFunc;
        std::function<core::Vector_t(core::Vector_t const&)> coerceIn;
        core::structural::IndicesGenerator indicesGenerator;
        core::LowerBoundOfNode lowerBoundOfNode;
        std::function<bool(core::Vector_t const&)> isInProvidedBounds;

        TaskHolder(
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
        );
    };

    struct TaskHolderAsync {
        int dimensionSize;
        int boundsSize;
        std::function<
            core::Vector_col_t(
                core::Vector_col_t const&,
                core::Matrix_t const&,
                core::Vector_col_t const&
            )
        > calculateDerivativeFunction;
        std::function<
            core::Vector_col_t(
                core::Vector_col_t const&,
                core::Matrix_t const&,
                core::Vector_col_t const&,
                core::Matrix_t const&
            )
        > calculateOriginalSolution;
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
        > recalculateBaseMatrices;
        std::function<
            core::Vector_col_t(
                core::Vector_col_t,
                core::Vector_col_t const&,
                core::Vector_col_t const&
            )
        > coerceIn;
        std::function<
            long double(
                core::Vector_col_t const&,
                core::Vector_col_t const&,
                core::Matrix_t const&
            )
        > cost;
        core::structural::IndicesGenerator indicesGenerator;
        core::LowerBoundOfNode lowerBoundOfNode;

        TaskHolderAsync(
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
        );
    };

    struct ArgumentsHolderAsync {
        core::Matrix_t A;
        core::Vector_col_t b;
        core::Vector_col_t const& d;
        core::Matrix_t const& Q;
        core::Vector_col_t const& alpha;
        core::Vector_col_t const& beta;
        core::Matrix_t C;
        core::Vector_col_t p;

        ArgumentsHolderAsync(
            core::Matrix_t const& A,
            core::Vector_col_t const& b,
            core::Vector_col_t const& d,
            core::Matrix_t const& Q,
            core::Vector_col_t const& alpha,
            core::Vector_col_t const& beta,
            core::Matrix_t  const& C,
            core::Vector_col_t const& p
        );

        ArgumentsHolderAsync(ArgumentsHolderAsync const& other) = default;
        ArgumentsHolderAsync& operator=(ArgumentsHolderAsync const& other) = default;
    };

namespace core {

    struct TreeNodeArguments {
        double fRec;
        Indices lowerBound, upperBound;
        structural::IndicesGenerator generator;

        TreeNodeArguments(
            double fRec,
            Indices lowerBound,
            Indices upperBound,
            structural::IndicesGenerator generator
        ): fRec { fRec },
           lowerBound { std::move(lowerBound) },
           upperBound { std::move(upperBound) },
           generator { std::move(generator) }
        {}

        std::string toString() const {
            return "lowerBound: " + lowerBound.toString() +
                ", upperBound: " + upperBound.toString() +
                ", generator: " + generator.toString() + "\n";
        }
    };

    class Backend {

        int dimensionSize, boundsSize;
        std::function<Vector_t(Vector_t const&)> calculateDerivativeFunction;
        std::function<double(Vector_t const&, Vector_t const&)> calculateAlphaParam;
        std::function<Vector_t(Vector_t const&)> calculateOriginalSolution;
        std::function<void(Indices const&, Indices const&)> recalculateBaseMatrices;
        std::function<double(Vector_t const&)> calculateCostFunction;
        std::function<double(core::Vector_t const&)> assistiveTaskCostFunc;
        std::function<double(core::Vector_t const&, int const)> conjugatePartialDerivativeFunc;
        std::function<core::Vector_t(core::Vector_t const&)> coerceIn;
        structural::IndicesGenerator indicesGenerator;
        LowerBoundOfNode lowerBoundOfNode;
        std::function<bool(core::Vector_t const&)> isInProvidedBounds;

        public:
        Backend(babp::TaskHolder &&taskHolder);
        Backend(babp::TaskHolder const& taskHolder);

        Vector_t solve(std::ostream &output) const;
    };

    class BackendAsync {

        int dimensionSize;
        int boundsSize;
        std::function<
            core::Vector_col_t(
                core::Vector_col_t const&,
                core::Matrix_t const&,
                core::Vector_col_t const&
            )
        > calculateDerivativeFunction;
        std::function<
            core::Vector_col_t(
                core::Vector_col_t const&,
                core::Matrix_t const&,
                core::Vector_col_t const&,
                core::Matrix_t const&
            )
        > calculateOriginalSolution;
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
        > recalculateBaseMatrices;
        std::function<
            core::Vector_col_t(
                core::Vector_col_t,
                core::Vector_col_t const&,
                core::Vector_col_t const&
            )
        > coerceIn;
        std::function<
            long double(
                core::Vector_col_t const&,
                core::Vector_col_t const&,
                core::Matrix_t const&
            )
        > cost;
        core::structural::IndicesGenerator indicesGenerator;
        core::LowerBoundOfNode lowerBoundOfNode;

        std::mutex solutionMutex;
        long double result = __DBL_MAX__;
        Vector_col_t solution;

        void _solve(
            TreeNodeArguments node,
            babp::ArgumentsHolderAsync arguments,
            ConjugateCoordDescent const& descent,
            BabpThreadPool &threadPool
        );

        public:
        BackendAsync(babp::TaskHolderAsync &&taskHolder);
        BackendAsync(babp::TaskHolderAsync const& taskHolder);

        Vector_col_t solve(
            babp::ArgumentsHolderAsync holder,
            int const threadCount,
            std::ostream &output
        );
    };

} // namespace core
} // namespace babp