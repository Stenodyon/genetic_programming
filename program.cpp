#include <cmath>
#include <cstring>
#include <iostream>
#include <random>

#include "program.hpp"

tree_ptr<Symbol> random_tree()
{
    return Symbol::random_tree(Symbol::types::str_t);
}

double fitness(tree_ptr<Symbol> tree)
{
    static const std::string target = "Hello";
    auto str = std::unique_ptr<std::string>(
            (std::string*) Symbol::evaluate(tree));
    unsigned int diff = strcmp(target.c_str(), str->c_str());
    return 1.0 / (diff + 1.0);
}
