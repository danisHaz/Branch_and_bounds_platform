#include "inflater.hpp"
#include <iostream>

int main() {

    babp::core::Inflater inflater;
    std::cout << inflater.inflate("-a * (b + c)");

    return 0;
}