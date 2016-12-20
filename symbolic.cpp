#include <cmath>
#include <iostream>
#include <memory>
#include <random>

#include "tree.hpp"
#include "optimizer.hpp"

const double target_fitness = 0.999;
const unsigned int population_size = 100;

enum class math_type
{
    number,
    boolean
};

enum class sym_t
{
    x,
    plus,
    one,
    equals
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
                case sym_t::equals:
                    os << "==";
                    break;
                default:
                    break;
            }
            return os;
        }
};

tree_ptr<Symbol,math_type> random_numerical_expression()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0,1);
    double random = dis(gen);
    if(random < 0.3)
    {
        return std::make_shared<Tree<Symbol,math_type>>(
                Symbol(sym_t::x),
                math_type::number);
    }
    else if(random < 0.6)
    {
        auto child1 = random_numerical_expression();
        auto child2 = random_numerical_expression();
        auto tree = std::make_shared<Tree<Symbol,math_type>>(
                Symbol(sym_t::plus),
                math_type::number);
        tree->add(child1);
        tree->add(child2);
        return tree;
    }
    else
    {
        return std::make_shared<Tree<Symbol,math_type>>(
                Symbol(sym_t::one),
                math_type::number);
    }
}

tree_ptr<Symbol,math_type> random_tree()
{
    auto tree = std::make_shared<Tree<Symbol,math_type>>(
            Symbol(sym_t::equals),
            math_type::boolean);
    tree->add(random_numerical_expression());
    tree->add(random_numerical_expression());
    return tree;
}

double evaluate(tree_ptr<Symbol,math_type> tree, double x_value)
{
    switch(tree->get_node().get_type())
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

double fitness(tree_ptr<Symbol,math_type> tree)
{
    double val11 = evaluate(tree->get_children()[0], 1.0);
    double val12 = evaluate(tree->get_children()[1], 1.0);
    double error1 = abs(val11 - val12);
    double val21 = evaluate(tree->get_children()[0], 2.0);
    double val22 = evaluate(tree->get_children()[1], 2.0);
    double error2 = abs(val21 - val22);
    double val3 = evaluate(tree->get_children()[0], 3.0);
    double error3 = abs(val3 - 10.0);
    double error = error1 + error2 + error3;
    return 1.0 / (error + 1.0);
}

unsigned int count_type(tree_ptr<Symbol,math_type> tree, sym_t type)
{
    sym_t t = tree->get_node().get_type();
    if(t == type)
    {
        return 1;
    }
    else if(t == sym_t::plus)
    {
        unsigned int count1 = count_type(tree->get_children()[0], type);
        unsigned int count2 = count_type(tree->get_children()[1], type);
        return count1 + count2;
    }
    else
    {
        return 0;
    }
}

int main()
{
    Optimizer<Symbol,math_type> opt(&fitness, &random_tree, population_size);
    tree_ptr<Symbol,math_type> best = opt.run_until_fitness(target_fitness);
    std::cout << "Best tree:" << std::endl;
    std::cout << *best << std::endl;
    return 0;
}
