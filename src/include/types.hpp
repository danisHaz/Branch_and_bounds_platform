#pragma once

#include <Dense>

#include <variant>
#include <optional>

namespace babp {
namespace core {
    template < int colsNumber >
    using Vector_st_t = Eigen::Matrix<double, 1, colsNumber>;

    template < int colsNumber >
    using Vector_col_st_t = Eigen::Matrix<double, colsNumber, 1>;

    template < int colsNumber, int rowsNumber >
    using Matrix_st_t = Eigen::Matrix<double, rowsNumber, colsNumber>;

    using Vector_t = Eigen::Matrix<double, 1, Eigen::Dynamic>;
    using Vector_col_t = Eigen::Matrix<double, Eigen::Dynamic, 1>;
    using Matrix_t = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
    using Matrix_costyl_t = Eigen::Matrix<double, 1, 1>;

    using Integral_var_t = std::variant<
        std::optional<int>,
        std::optional<double>
    >;

    using Matrix_var_t = std::variant<
        std::optional<Vector_t>,
        std::optional<Vector_col_t>,
        std::optional<Matrix_t>
    >;
} // namespace babp
} // namespace core