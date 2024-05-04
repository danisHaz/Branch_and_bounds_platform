#include "solution.hpp"
#include "backend.hpp"

Eigen::VectorXd babp::solver::solve(TaskHolder const& task, std::ostream &output) {
    core::Backend backend { task };
    return backend.solve(output);
}

Eigen::VectorXd babp::solver::solve(TaskHolder &&task, std::ostream &output) {
    core::Backend backend { std::move(task) };
    return backend.solve(output);
}

Eigen::VectorXd babp::solver::solve(
    TaskHolderAsync const& task,
    ArgumentsHolderAsync arguments,
    int const threadCount,
    std::ostream &output
) {
    core::BackendAsync backend { task };
    return backend.solve(arguments, threadCount, output);
}

Eigen::VectorXd babp::solver::solve(
    TaskHolderAsync &&task,
    ArgumentsHolderAsync arguments,
    int const threadCount,
    std::ostream &output
) {
    core::BackendAsync backend { std::move(task) };
    return backend.solve(arguments, threadCount, output);
}
