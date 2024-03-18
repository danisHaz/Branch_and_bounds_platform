#include <utility>
#include <stack>
#include <cassert>

#include <boost/algorithm/string/erase.hpp>

#include "inflater.hpp"

using namespace babp::core;

std::vector<Group> Inflater::getGroupsList(std::string const& fStr) const {
    std::vector<Group> result;

    std::stack<Pointer> pointers;
    pointers.push(Pointer(0, PointerType::BEGIN));

    for (std::size_t pos = 0; pos < fStr.size(); pos++) {
        if (fStr[pos] == CIRCLE_PARENTH_OPEN) {
            pointers.push(Pointer(pos, PointerType::BEGIN));
        } else if (fStr[pos] == CIRCLE_PARENTH_CLOSE) {
            auto begin_pointer = pointers.top(); pointers.pop();
            result.push_back(Group(begin_pointer.position, pos));
        }
    }
    
    assert(pointers.size() == 1); // all opened brackets must be closed except our first one
    result.push_back(Group(0, static_cast<int>(fStr.size())));

    return result;
}

void Inflater::filterSpaces(std::string &fStr) const {
    boost::algorithm::erase_all(fStr, WHITESPACE);
}

void Inflater::inflate(std::string &functionStr) const {
    filterSpaces(functionStr);
}

std::string Inflater::inflate(std::string &&functionStr) const {
    std::string result { functionStr };
    filterSpaces(result);

    return std::move(result);
}