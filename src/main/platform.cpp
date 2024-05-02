#include <iostream>
#include <cmath>
#include <thread>

#include "boost/coroutine2/all.hpp"

#include "Dense"

#include "tasks.hpp"
#include "backend.hpp"

template < typename T >
using croutine_t = typename boost::coroutines2::coroutine<T>;

template < typename T >
using c_push_t = typename boost::coroutines2::coroutine<T>::push_type;

template < typename T >
using c_pull_t = typename boost::coroutines2::coroutine<T>::pull_type;

int main() {

    c_pull_t<void> routine{ [](c_push_t<void> &p) {
        std::cout << "meme\n";
        p();

        std::cout << "unmeme\n";
    } };

    routine();

    std::cout << "keke\n";

    return 0;
}