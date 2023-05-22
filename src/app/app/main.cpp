#include <iostream>
#include <libset/treeset.hpp>

int main() {
    treeset::Set<int> set;
    // std::cout << "pisua" << " ";
    set.add(10);
    set.add(12);
    set.add(14);
    set.add(1);
    set.add(13);
    set.add(15);
    set.add(12);
    set.add(-10);
    set.add(177);
    set.add(153);
    set.add(-1000);
    set.add(1);
    set.erase(-1000);
    set.erase(1);
    set.erase(14);
    set.erase(14);
    set.erase(12);
    set.erase(1250);
    set.clear();
    // set.print_tree();
}