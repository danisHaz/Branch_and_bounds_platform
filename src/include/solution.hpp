#pragma once

#include <iostream>

#include "backend.hpp"

namespace babp {
namespace solver {

    Eigen::VectorXd solve(TaskHolder const& task, std::ostream &output = std::cout);
    Eigen::VectorXd solve(TaskHolder &&task, std::ostream &output = std::cout);
} // namespace solver
} // namespace babp