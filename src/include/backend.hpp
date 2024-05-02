#pragma once

#include <functional>
#include <iostream>

#include "types.hpp"
#include "index_bound_set.hpp"
#include "algorithm.hpp"

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

namespace core {
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
        Vector_t solveAsync(std::ostream &output);
    };

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

} // namespace core
} // namespace babp