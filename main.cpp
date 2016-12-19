#include <iostream>
#include <memory>
#include <random>

#include "tree.hpp"
#include "optimizer.hpp"

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
    std::cout << "Computing fitness" << std::endl;
    double val = 0;
    const std::vector<tree_ptr<bool>> & children = tree->get_children();
    if(children.size() >= 2)
    {
        std::cout << children.size() << " children" << std::endl;
        if(*(children[0]->get_node()) == false)
            val += 1;
        if(*(children[1]->get_node()) == true)
            val += 1;
        val += fitness(children[0]);
        val += fitness(children[1]);
    }
    std::cout << "Fitness = " << val << std::endl;
    return val;
}

int main()
{
    auto tree = random_tree();
    std::cout << *tree << std::endl;
    double fit = fitness(tree);
    std::cout << fit << std::endl;
    Optimizer<bool> opt(&fitness, &random_tree, 10);
    opt.run();
    return 0;
}
