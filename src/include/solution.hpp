#pragma once

#include <functional>

#include "types.hpp"
#include "index_bound_set.hpp"
#include "algorithm.hpp"
#include "backend.hpp"

namespace babp {

    class Solver {

        Eigen::VectorXd solve(TaskHolder const& task) const {
            core::Backend backend { task };
            return backend.solve();
        }

        Eigen::VectorXd solve(TaskHolder &&task) const {
            core::Backend backend { std::move(task) };
            return backend.solve();
        }
    };

} // namespace babp