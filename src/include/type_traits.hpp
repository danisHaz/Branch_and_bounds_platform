#pragma once

#include <Dense>

#include <variant>
#include <optional>

namespace babp {
namespace core {
	
    template < typename T, typename... Args >
    struct prolong_variant;

    template < typename... Args0, typename... Args1 >
    struct prolong_variant<std::variant<Args0...>, Args1...> {
        using value = std::variant<Args0..., Args1...>;
    };

    template < typename T, typename U >
    struct combine_variants;

    template < typename... Args0, typename... Args1 >
    struct combine_variants<std::variant<Args0...>, std::variant<Args1...>> {
        using value = std::variant<Args0..., Args1...>;
    };

    template < typename T, typename U, typename... args >
    constexpr std::size_t getSameTypeIndex(std::size_t ind = 0) {
        if constexpr (std::is_same_v<std::decay_t<T>, std::decay_t<U>>) {
            return ind;
        } else {
            if constexpr (sizeof...(args) == 0) {
                return ind + 1;
            } else {
                return getSameTypeIndex<T, args...>(ind + 1);
            }
        }
    }

    template < typename T, typename Variant >
    struct contains_type;

    template < typename T, typename... Args >
    struct contains_type<T, std::variant<Args...>> {
        static constexpr bool value = getSameTypeIndex<T, Args...>() != sizeof...(Args) ? true : false;
    };

    using Vector_t = Eigen::Matrix<double, 1, Eigen::Dynamic>;
    using Vector_col_t = Eigen::Matrix<double, Eigen::Dynamic, 1>;
	using Matrix_t = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

    using Integral_var_t = std::variant<
        std::optional<int>,
        std::optional<double>
    >;

    using Matrix_var_t = std::variant<
        std::optional<Vector_t>,
        std::optional<Vector_col_t>,
        std::optional<Matrix_t>
    >;

    using Var_t = combine_variants<Integral_var_t, Matrix_var_t>::value;

    template < typename T >
    constexpr bool isIntegralType() {
        return contains_type<T, Integral_var_t>::value ||
            contains_type<std::optional<T>, Integral_var_t>::value;
    }

    template < typename T >
    constexpr bool isMatrixType() {
        return contains_type<T, Matrix_var_t>::value ||
            contains_type<std::optional<T>, Matrix_var_t>::value;
    }

    template < typename T, typename U >
    constexpr bool isSameTypes() {
        return std::is_same_v<T, U>;
    }

} // namespace core
} // namespace babp