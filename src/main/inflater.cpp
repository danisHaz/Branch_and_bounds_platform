#include <utility>
#include <stack>
#include <cassert>

#include <boost/algorithm/string/erase.hpp>

#include "inflater.hpp"
#include "core_constants.hpp"

using namespace babp::core;

std::vector<Group> Inflater::getGroupsList(std::string const& fStr) const {
    std::vector<Group> result;

    std::stack<Pointer> pointers;
    pointers.push(Pointer(0, PointerType::BEGIN));

    for (std::size_t pos = 0; pos < fStr.size(); pos++) {
        if (fStr[pos] == CIRCLE_BRACKET_OPEN) {
            pointers.push(Pointer(pos + 1, PointerType::BEGIN));
        } else if (fStr[pos] == CIRCLE_BRACKET_CLOSE) {
            auto begin_pointer = pointers.top(); pointers.pop();
            result.push_back(Group(begin_pointer.position, pos));
        }
    }
    
    assert(pointers.size() == 1); // all opened brackets must be closed except our first one
    result.push_back(Group(0, static_cast<int>(fStr.size() - 1)));

    return result;
}

std::vector<Statement> Inflater::parseGroupToOperations(std::string const& fStr, Group const& group) const {
    assert(group.start < group.end);

    std::vector<Statement> result;
    std::size_t variableNameBegin = static_cast<std::size_t>(0);

    for (std::size_t i = group.start; i < group.end; i++) {
        if (auto statement = getOperationOrNull(fStr[i])) {
            pushVariableName(fStr, Group { variableNameBegin, i }, result);
            result.push_back(statement.value());
            variableNameBegin = i + 1;
        }
    }

    pushVariableName(fStr, Group { variableNameBegin, group.end }, result);

    return result;
}

void Inflater::pushVariableName(
    std::string const& fStr,
    Group const nameRange,
    std::vector<Statement>& sequence
) const {
    if (nameRange.start < nameRange.end) {
        sequence.push_back(
            VARIABLE_NAME_SIGN + fStr.substr(nameRange.start, nameRange.end - nameRange.start)
        );
    }
}

std::optional<structural::OperationType> Inflater::getOperationOrNull(char const c) const {
    using namespace structural;
    switch (c) {
        case MINUS:
            return OperationType::MINUS;
            break;
        
        case PLUS:
            return OperationType::PLUS;
            break;
        
        case MULT:
            return OperationType::MULT;
            break;
        
        case DIV:
            return OperationType::DIV;
            break;

        case COMMA:
            return OperationType::SCALAR_DIVIDER;
            break;
        
        case CIRCLE_BRACKET_OPEN:
            return OperationType::GROUP_START;
            break;
        
        case CIRCLE_BRACKET_CLOSE:
            return OperationType::GROUP_END;
            break;
        
        case ANGLE_BRACKET_OPEN:
            return OperationType::SCALAR_START;
            break;

        case ANGLE_BRACKET_CLOSE:
            return OperationType::SCALAR_END;
            break;

        default:
            return std::nullopt;
            break;
    }
}

std::string Inflater::filterSpaces(std::string const& fStr) const {
    return boost::algorithm::erase_all_copy(fStr, std::string { WHITESPACE } );
}

void Inflater::filterSpaces(std::string &fStr) const {
    return boost::algorithm::erase_all(fStr, std::string { WHITESPACE } );
}

std::vector<Statement> Inflater::inflate(std::string const& functionStr) const {
    return getOperationList(filterSpaces(functionStr));
}

std::vector<Statement> Inflater::inflate(std::string &&functionStr) const {
    std::string func { functionStr };
    filterSpaces(func);

    return getOperationList(func);
}

std::vector<Statement> Inflater::getOperationList(std::string const& fStr) const {
    std::vector<Statement> statements;
    for (auto &statement : parseGroupToOperations(fStr, Group { 0, fStr.size() })) {
        statements.push_back(std::move(statement));
    }

    return statements;
}

void Inflater::printStatement(std::ostream &output, Statement const& statement) const {
    std::visit([&output] (auto &&st) {
        using T = std::decay_t<decltype(st)>;

        if constexpr (std::is_same_v<T, std::string>) {
            output << st << " ";
        } else if constexpr (std::is_same_v<T, babp::core::structural::OperationType>) {
            output << babp::core::structural::meme[st] << " ";
        }
    }, statement);
}