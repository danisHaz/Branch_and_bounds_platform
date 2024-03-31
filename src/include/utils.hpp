#pragma once

namespace babp {
namespace core {

    void warning(char const* message);

    template < bool const condition, bool const... conditions >
    void warningIfAll(char const* message) {
        if (condition) {
            if constexpr (sizeof...(conditions) == 0) {
                warning(message);
            } else {
                warningIfAll<conditions...>(message);
            }
        }
    }

    template < bool const condition, bool const... conditions >
    void warningIfAny(char const* message) {
        if (condition) {
            warning(message);
        } else {
            if constexpr (sizeof...(conditions) != 0) {   
                warningIfAny<conditions...>(message);
            }
        }
    }

} // namespace core
} // namespace babp