#pragma once

#include <iostream>

#include "backend.hpp"

namespace babp {
namespace solver {

    Eigen::VectorXd solve(TaskHolder const& task, std::ostream &output = std::cout);
    Eigen::VectorXd solve(TaskHolder &&task, std::ostream &output = std::cout);
    Eigen::VectorXd solve(
        TaskHolderAsync const& task,
        ArgumentsHolderAsync arguments,
        int const threadCount,
        std::ostream &output
    );
    Eigen::VectorXd solve(
        TaskHolderAsync &&task,
        ArgumentsHolderAsync arguments,
        int const threadCount,
        std::ostream &output
    );
} // namespace solver
} // namespace babp