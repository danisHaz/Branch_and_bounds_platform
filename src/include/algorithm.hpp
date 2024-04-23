#pragma once

#include <cassert>
#include <optional>
#include <variant>
#include <vector>
#include <utility>
#include <map>
#include <memory>
#include <cstdint>

#include <Dense>

#include "type_traits.hpp"
#include "operations.hpp"
#include "utils.hpp"
#include "functional_computable.hpp"

namespace babp {
namespace core {
namespace structural {

    struct Variable : public FunctionalComputable {

        private:
        Var_t value;
        std::string name, key;

        public:
        Variable(
            Var_t &&value,
            std::string name,
            std::string key
        ): 
            value { std::move(value) },
            name { std::move(name) },
            key { std::move(key) } {}

        Variable(
            Var_t const& value,
            std::string name,
            std::string key
        ): 
            value { value },
            name { std::move(name) },
            key { std::move(key) } {}

        void setValue(Var_t const& newValue) {
            value = newValue;
        }

        void setValue(Var_t &&newValue) {
            value = newValue;
        }

        Var_t compute() const override {
            return value;
        }
    };

    static std::map<OperationType, std::string> meme {
        { OperationType::PLUS, "PLUS" },
        { OperationType::MINUS, "MINUS" },
        { OperationType::MULT, "MULT" },
        { OperationType::DIV, "DIV" },
        { OperationType::SCALAR_START, "SCALAR_START" },
        { OperationType::SCALAR_DIVIDER, "SCALAR_DIVIDER" },
        { OperationType::SCALAR_END, "SCALAR_END" },
        { OperationType::GROUP_START, "GROUP_START" },
        { OperationType::GROUP_END, "GROUP_END" },
    };

    class StructuralTreeNode : public FunctionalComputable {

        std::unique_ptr<FunctionalComputable> leftArg;
        std::unique_ptr<FunctionalComputable> rightArg;

        const OperationType operation;

        public:

        StructuralTreeNode(
            OperationType operation,
            std::unique_ptr<FunctionalComputable> &&leftArg,
            std::unique_ptr<FunctionalComputable> &&rightArg
        ): leftArg { std::move(leftArg) }, rightArg { std::move(rightArg) }, operation { operation } {}        

        Var_t compute() const override {
            Var_t result;
            std::visit([this, &result](auto &&left, auto &&right) {
                if (left.has_value() && right.has_value()) {
                    result = babp::core::compute(left, right, this->operation);
                }
            }, leftArg->compute(), rightArg->compute());

            return result;
        }
    };
} // namespace structural

} // namespace core
} // namespace babp