#include <cmath>
#include <iostream>
#include <memory>
#include <random>

#include "tree.hpp"
#include "optimizer.hpp"

const double target_fitness = 0.999;
const unsigned int population_size = 100;

enum class sym_t
{
    x,
    plus,
    one
};

class Symbol
{
    private:
        sym_t type;
    public:
        Symbol(sym_t type) : type(type) {}
        Symbol(const Symbol &copy) : type(copy.type) {}

        const sym_t & get_type() const { return type; }

        friend std::ostream & operator<<(std::ostream &os, const Symbol &sym)
        {
            switch(sym.type)
            {
                case sym_t::x:
                    os << "x";
                    break;
                case sym_t::plus:
                    os << "plus";
                    break;
                case sym_t::one:
                    os << "one";
                    break;
                default:
                    break;
            }
            return os;
        }
};

tree_ptr<Symbol> random_tree()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0,1);
    double random = dis(gen);
    if(random < 0.3)
    {
        auto x = std::make_shared<Symbol>(sym_t::x);
        return std::make_shared<Tree<Symbol>>(x);
    }
    else if(random < 0.6)
    {
        auto child1 = random_tree();
        auto child2 = random_tree();
        auto plus = std::make_shared<Symbol>(sym_t::plus);
        auto tree = std::make_shared<Tree<Symbol>>(plus);
        tree->add(child1);
        tree->add(child2);
        return tree;
    }
    else
    {
        auto one = std::make_shared<Symbol>(sym_t::one);
        return std::make_shared<Tree<Symbol>>(one);
    }
}

double evaluate(tree_ptr<Symbol> tree, double x_value)
{
    switch(tree->get_node()->get_type())
    {
        case sym_t::x:
            return x_value;
        case sym_t::plus:
            {
                double val1 = evaluate(tree->get_children()[0], x_value);
                double val2 = evaluate(tree->get_children()[1], x_value);
                return val1 + val2;
            }
        case sym_t::one:
            return 1.0;
        default:
            return 0.0;
    }
}

double fitness(tree_ptr<Symbol> tree)
{
    static const double target = 10.0;
    double val1 = evaluate(tree, 1.0);
    double error = abs(val1 - target);
    return 1.0 / (error + 1.0);
}

int main()
{
    Optimizer<Symbol> opt(&fitness, &random_tree, population_size);
    tree_ptr<Symbol> best = opt.run_until_fitness(target_fitness);
    std::cout << "Best tree:" << std::endl;
    std::cout << *best << std::endl;
    std::cout << "best(1) = " << evaluate(best, 1.0) << std::endl;
    return 0;
}
