#include <functional>

#include "index_bound_set.hpp"

babp::core::Indices::Indices(int indicesMaxCount, std::uint64_t indices): indicesMaxCount { indicesMaxCount }, _indices { indices } {}
babp::core::Indices::Indices(int indicesMaxCount, std::vector<int> const& indices): indicesMaxCount { indicesMaxCount } {
    for (int const& index: indices) {
        _indices |= (1 << index);
    }
}

babp::core::Indices babp::core::Indices::setIndex(int newIndex) const {
    return babp::core::Indices { indicesMaxCount, _indices | (1 << newIndex) };
}

babp::core::Indices babp::core::Indices::removeIndex(int removeIndex) const {
    return babp::core::Indices { indicesMaxCount, _indices ^ (_indices & (1 << removeIndex)) };
}

void babp::core::Indices::iterateOver(std::function<void(int)> const& onNewIndex) const {
    for (int index = 1, step = 0; index < indicesMaxCount; index <<= 1, step++) {
        if ((_indices & index) == 0) {
            continue;
        }

        onNewIndex(step);
    }
}