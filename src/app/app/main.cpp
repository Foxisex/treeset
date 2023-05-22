#include <iostream>
#include <libset/treeset.hpp>

int main() {
    treeset::Set<int> set;
    // std::cout << "pisua" << " ";
    set.insert(10);
    set.insert(12);
    set.insert(14);
    set.insert(1);
    set.insert(13);
    set.insert(15);
    set.insert(12);
    set.insert(-10);
    set.insert(177);
    set.insert(153);
    set.insert(-1000);
    set.insert(1);

    for (int i = 0; i < 100; i++) {
        set.insert(i);
    }  // for(const auto node : set) {
    // 	std::cout << node->key << std::endl;
    // }
    // for(auto it = set.end(); it!= set.begin(); --it) {
    // 	std::cout << (*it) << " ";
    // }

    auto setptr = set.find(12);
    setptr++;
    std::cout << *setptr << std::endl;
    ++setptr;
    std::cout << *setptr << std::endl;
    setptr--;
    std::cout << *setptr << std::endl;
    --setptr;
    std::cout << *setptr << std::endl;

    //  for(auto it = set.begin(); it!= set.end(); ++it) {
    //     std::cout << (*it) << " ";
    // }
    // set.erase(-1000);
    std::cout << set.size() << std::endl;
    // set.erase(1);
    // set.erase(14);
    // set.erase(14);
    // set.erase(12);
    // set.erase(1250);
    // set.clear();
    // set.print_tree();
}