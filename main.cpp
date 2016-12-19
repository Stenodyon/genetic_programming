#include <iostream>
#include <memory>

#include "tree.hpp"

void print_position(pos & position)
{
    for(unsigned int i : position)
        std::cout << i << ".";
    std::cout << "nil" << std::endl;
}

int main()
{
    auto one = std::make_shared<int>(1);
    auto two = std::make_shared<int>(2);
    auto six = std::make_shared<int>(6);
    auto t1 = std::make_shared<Tree<int>>(one);
    auto t6 = std::make_shared<Tree<int>>(six);
    auto t = std::shared_ptr<Tree<int>>(new Tree<int>(two, {t1}));
    std::list<unsigned int> l{0};
    t->replace(t6, l);
    std::cout << *t << std::endl;
    auto pos = t->random_position();
    print_position(pos);
    return 0;
}
