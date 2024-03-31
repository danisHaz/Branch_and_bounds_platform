#pragma once

#include <cassert>
#include <optional>
#include <variant>
#include <vector>
#include <utility>
#include <map>

#include <Dense>

#include "type_traits.hpp"

namespace babp {
namespace core {

namespace structural {

    template < typename ComputationResult >
    class FunctionalComputable {

        virtual ComputationResult compute() const = 0;
    };

    struct Variable : FunctionalComputable<Var_t> {

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

    enum class OperationType {
        PLUS, MINUS, MULT, DIV, SCALAR_START, SCALAR_DIVIDER, SCALAR_END, GROUP_START, GROUP_END
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

    template < typename data_type >
    class StructuralTreeNode : public FunctionalComputable<data_type> {

        std::vector<FunctionalComputable<data_type>> arguments;
        OperationType operation;

        data_type computePlus() const {
            assert(arguments.size() == 2);
            return arguments[0].compute() + arguments[1].compute();
        }
        
        data_type computeMinus() const {
            assert(arguments.size() == 2);
            return arguments[0].compute() - arguments[1].compute();
        }

        data_type computeMult() const {
            assert(arguments.size() == 2);
            return arguments[0].compute() * arguments[1].compute();
        }

        data_type computeDiv() const {
            assert(arguments.size() == 2);
            assert(arguments[1] != 0);
            return arguments[0].compute() / arguments[1].compute();
        }

        data_type computeUnaryMinus() const {
            assert(arguments.size() == 1);
            return -arguments[0].compute();
        }

        public:
        StructuralTreeNode(OperationType operation, FunctionalComputable... args): arguments { args... }, operation { operation } {}

        data_type compute() const override {
            if (operation == OperationType::PLUS) {
                return computePlus();
            }
        }
    };
} // namespace structural

} // namespace core
} // namespace babp