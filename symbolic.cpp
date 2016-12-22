#include <cmath>
#include <iostream>
#include <memory>
#include <random>

#include "tree.hpp"
#include "optimizer.hpp"
#include "program.hpp"

int main()
{
#if 0
    auto tree = random_tree();
    std::cout << *tree << std::endl;
    auto str = std::unique_ptr<std::string>(
            (std::string*) Symbol::evaluate(tree));
    std::cout << *str << std::endl;
#endif
#if 1
    Optimizer<Symbol> opt(&fitness, &random_tree, population_size);
    tree_ptr<Symbol> best = opt.run_until_fitness(target_fitness);
    std::cout << "Best tree:" << std::endl;
    std::cout << *best << std::endl;
#endif
    return 0;
}
