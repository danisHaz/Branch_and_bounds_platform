#pragma once

#include <functional>
#include <cstdint>
#include <vector>
#include <memory>

#include "utils.hpp"
#include "functional_computable.hpp"

namespace babp {
namespace core {


    /**
     * Bound represents additional function
     * classic - is checking f(arg) >= 0 (when true) or f(arg) <= 0 (when false)
    */
    template < typename T >
    class Bound {
        std::unique_ptr<structural::FunctionalComputable> f;
        bool classic;

        public:
        Bound(std::unique_ptr<structural::FunctionalComputable> &&func, bool classic): f { std::move(func) }, classic { classic } {}

        bool check(T arg) {
            bool checkResult;
            std::visit([this, &checkResult](auto &&result) {
                if (isIntegralType<decltype(result)>()) {
                    if (classic) {
                        checkResult = result >= 0;
                    } else {
                        checkResult = result <= 0;
                    }
                } else {
                    checkResult = false;
                }
            }, f->compute());

            return checkResult;
        }

        Var_t compute() {
            return f->compute();
        }
    };

    template <>
    class Bound<void> {
        std::function<bool()> _check;

        public:
        Bound(std::function<bool()> &&check): _check { std::move(check) } {}
        Bound(std::function<bool()> const &check): _check { check } {}

        bool check() const {
            return _check();
        }
    };

    template < typename Arg_t >
    class IndexBoundSet {
        std::vector<Bound<Arg_t>> _bounds;
        std::vector<std::string> keys;

        public:
        IndexBoundSet(std::vector<Bound<Arg_t>> &&bounds): _bounds { bounds } {
            for (std::size_t ind = 0; ind < _bounds.size(); ind++) {
                keys.push_back(randomKey());
            }
        }

        std::string const& getKey(std::size_t index) const {
            if (index > keys.size()) {
                throw std::logic_error("index of key cannot be greater than number of bounds");
            }

            return keys[index];
        }
    };

    class Indices {
        int indicesMaxCount;
        std::int64_t _indices;

        public:
        Indices(int indicesMaxCount, std::int64_t indices);
        Indices(int indicesMaxCount, std::vector<int> const& indices);

        Indices setIndex(int newIndex) const;
        Indices removeIndex(int removeIndex) const;
        void iterateOver(std::function<void(int)> const& onEach) const;
        void iterateOverNonExisting(std::function<void(int)> const& onEach) const;
    };

} // namespace core
} // namespace babp