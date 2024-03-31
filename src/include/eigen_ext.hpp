#pragma once

#include "type_traits.hpp"

#include <Dense>

namespace babp {
namespace core {

    bool canBeMultiplied(Matrix_t left, Matrix_t right) {
        return left.cols() == right.rows();
    }

    bool canBeMultiplied(Vector_t left, Matrix_t right) {
        return left.cols() == right.rows();
    }

    bool canBeMultiplied(Matrix_t left, Vector_col_t right) {
        return left.cols() == right.rows();
    }

} // namespace core
} // namespace babp