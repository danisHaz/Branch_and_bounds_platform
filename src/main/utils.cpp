#include "utils.hpp"

#include <iostream>
#include <random>

void babp::core::warning(char const* message) {
    std::cout << message << "\n";
}

std::mt19937 mt;

auto babp::core::getRandom() {
    return mt();
}

std::string babp::core::randomKey(std::size_t length) {
    char *key = new char[length+1];
    for (int ind = 0; ind < length; ind++) {
        key[ind] = char((getRandom() % KEY_ALPHABET_LEN) + KEY_ALPHABET_START);
    }
    key[length] = '\0';

    return std::string { key, length };
}