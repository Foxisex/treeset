#include <iostream>
#include <libset/treeset.hpp>

int main() {
    treeset::Set<int> set{2, 2, 2, 0, 4, 1, 2, 3, 9, 8, 7, 6, 5};

    for (const auto& sElement : set) {
        std::cout << sElement * 2 << std::endl;
    }
}