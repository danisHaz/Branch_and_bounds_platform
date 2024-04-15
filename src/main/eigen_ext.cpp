#include "eigen_ext.hpp"
#include "type_traits.hpp"

using namespace babp::core;

bool babp::core::canBeMultiplied(Matrix_t left, Matrix_t right) {
    return left.cols() == right.rows();
}