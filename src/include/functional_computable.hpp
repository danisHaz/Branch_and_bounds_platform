#pragma once

#include "type_traits.hpp"

namespace babp {
namespace core {
namespace structural {

    class FunctionalComputable {

        public:
        virtual Var_t compute() const = 0;
    };

} // namespace structural
} // namespace core
} // namespace babp