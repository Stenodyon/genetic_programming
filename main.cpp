#include <iostream>
#include <memory>
#include <random>

#include "tree.hpp"
#include "optimizer.hpp"

const unsigned int steps = 100;
const unsigned int population_size = 100;
const double target_fitness = 15.0;

void print_position(pos & position)
{
    for(unsigned int i : position)
        std::cout << i << ".";
    std::cout << "nil" << std::endl;
}

std::shared_ptr<bool> mkb(bool b)
{
    return std::make_shared<bool>(b);
}

tree_ptr<bool> random_tree()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0,1);
    if(dis(gen) < 0.5)
        return std::make_shared<Tree<bool>>(mkb(dis(gen) < 0.5));
    auto tree1 = std::make_shared<Tree<bool>>(mkb(dis(gen) < 0.5));
    auto tree2 = std::make_shared<Tree<bool>>(mkb(dis(gen) < 0.5));
    std::vector<tree_ptr<bool>> l = {tree1, tree2};
    return std::make_shared<Tree<bool>>(mkb(dis(gen) < 0.5), l);
}

double fitness(tree_ptr<bool> tree)
{
    double val = 0;
    const std::vector<tree_ptr<bool>> & children = tree->get_children();
    if(children.size() >= 2)
    {
        if(*(children[0]->get_node()) == false)
            val += 1;
        else
            val -= 1;
        if(*(children[1]->get_node()) == true)
            val += 1;
        else
            val -= 1;
        val += fitness(children[0]);
        val += fitness(children[1]);
    }
    return val;
}

int main()
{
    Optimizer<bool> opt(&fitness, &random_tree, population_size);
    tree_ptr<bool> best = opt.run_until_fitness(target_fitness);
    std::cout << "Best tree:" << std::endl;
    std::cout << *best << std::endl;
    return 0;
}
