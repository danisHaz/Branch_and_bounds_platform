#pragma once

#include <cassert>
#include <optional>
#include <variant>
#include <vector>
#include <utility>

namespace babp {
namespace core {

    class Function {

        
    };

namespace structural {

    template < typename ComputationResult >
    class FunctionalComputable {
        
        virtual ComputationResult compute() const = 0;
    };

    template < typename T >
    struct Variable {

        T value;
        std::string name, key;

        Variable(std::string const& name, std::string const& key): name { name }, key { key } {}
        

        T compute() const override {
            return value;
        }
    };

    enum class OperationType {
        PLUS, MINUS, MULT, DIV, SCALAR, GROUP_START, GROUP_END
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