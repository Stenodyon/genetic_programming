#pragma once

#include <memory>

#include "tree.hpp"

template<typename T, typename node_type_t = typename T::types>
using tree_ptr = std::shared_ptr<Tree<T,node_type_t>>;

const double target_fitness = 0.999;
const unsigned int population_size = 100;

class Symbol
{
    public:
        enum class types
        {
            char_t,
            int_t,
            str_t,
            statement_t
        };
        enum class func
        {
            // str_t
            nil,
            cons,
            tl,
            // int_t
            int_const,
            plus,
            minus,
            ord,
            // char_t
            char_const,
            hd
        };
        Symbol(func type)
            : type(type), i_value(0), c_value(0) {}
        Symbol(func type, int i_value)
            : type(type), i_value(i_value), c_value(0) {}
        Symbol(func type, char c_value)
            : type(type), i_value(0), c_value(c_value) {}
        Symbol(const Symbol &copy)
            : type(copy.type), i_value(copy.i_value), c_value(copy.c_value) {}

        const func & get_type() const { return type; }

        static tree_ptr<Symbol> random_tree(types t)
        {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<> dis(0,1);
            switch(t)
            {
                case types::char_t:
                    if(dis(gen) < 0.5) // char_const
                    {
                        char random_char = (char)(dis(gen) * 256);
                        auto tree = std::make_shared<Tree<Symbol>>(
                                Symbol(func::char_const, random_char),t);
                        return tree;
                    }
                    else // hd
                    {
                        auto str = Symbol::random_tree(types::str_t);
                        auto tree = std::make_shared<Tree<Symbol>>(
                                Symbol(func::hd),t);
                        tree->add(str);
                        return tree;
                    }
                case types::int_t:
                    switch((char)(dis(gen) * 4))
                    {
                        case 0: // int_const
                            {
                                int random_int = (int)(dis(gen) * 256);
                                auto tree = std::make_shared<Tree<Symbol>>(
                                        Symbol(func::int_const, random_int),t);
                                return tree;
                            }
                        case 1: // plus
                            {
                                auto op1 = Symbol::random_tree(types::int_t);
                                auto op2 = Symbol::random_tree(types::int_t);
                                auto tree = std::make_shared<Tree<Symbol>>(
                                        Symbol(func::plus), t);
                                tree->add(op1);
                                tree->add(op2);
                                return tree;
                            }
                        case 2: // Minus
                            {
                                auto op1 = Symbol::random_tree(types::int_t);
                                auto op2 = Symbol::random_tree(types::int_t);
                                auto tree = std::make_shared<Tree<Symbol>>(
                                        Symbol(func::minus), t);
                                tree->add(op1);
                                tree->add(op2);
                                return tree;
                            }
                        case 3: // ord
                            {
                                auto c = Symbol::random_tree(types::char_t);
                                auto tree = std::make_shared<Tree<Symbol>>(
                                        Symbol(func::ord), t);
                                tree->add(c);
                                return tree;
                            }
                        default:
                            break;
                    }
                case types::str_t:
                    switch((char)(dis(gen) * 3))
                    {
                        case 0: // nil
                            {
                                auto tree = std::make_shared<Tree<Symbol>>(
                                        Symbol(func::nil),t);
                                return tree;
                            }
                        case 1: // cons
                            {
                                auto val = Symbol::random_tree(types::char_t);
                                auto tl  = Symbol::random_tree(types::str_t);
                                auto tree = std::make_shared<Tree<Symbol>>(
                                        Symbol(func::cons), t);
                                tree->add(val);
                                tree->add(tl);
                                return tree;
                            }
                        case 2: // tl
                            {
                                auto tl = Symbol::random_tree(types::str_t);
                                auto tree = std::make_shared<Tree<Symbol>>(
                                        Symbol(func::tl), t);
                                tree->add(tl);
                                return tree;
                            }
                        default:
                            break;
                    }
                default:
                    break;
            }
        }

        static void * evaluate(tree_ptr<Symbol> tree)
        {
            types t = tree->get_type();
            Symbol & node = tree->get_node();
            func function = node.get_type();
            switch(t)
            {
                case types::char_t:
                    if(function == func::char_const)
                    {
                        return new char(node.get_c_value());
                    }
                    else if(function == func::hd)
                    {
                        auto str = std::unique_ptr<std::string>(
                                (std::string*)
                                Symbol::evaluate(tree->get_children()[0]));
                        if(str->length() > 0)
                            return new char(str->at(0));
                        else
                            return new char(0);
                    }
                    else
                    {
                        std::cerr << *tree << std::endl << "Is of type char_t"
                            << " but was not either char_const nor hd"
                            << std::endl;
                        exit(-1);
                    }
                case types::int_t:
                    if(function == func::int_const)
                    {
                        return new int(node.get_i_value());
                    }
                    else if(function == func::plus)
                    {
                        auto val1 = std::unique_ptr<int>(
                                (int*)Symbol::evaluate(tree->get_children()[0]));
                        auto val2 = std::unique_ptr<int>(
                                (int*)Symbol::evaluate(tree->get_children()[1]));
                        return new int(*val1 + *val2);
                    }
                    else if(function == func::minus)
                    {
                        auto val1 = std::unique_ptr<int>(
                                (int*)Symbol::evaluate(tree->get_children()[0]));
                        auto val2 = std::unique_ptr<int>(
                                (int*)Symbol::evaluate(tree->get_children()[1]));
                        return new int(*val1 - *val2);
                    }
                    else if(function == func::ord)
                    {
                        auto c = std::unique_ptr<char>(
                                (char*)Symbol::evaluate(tree->get_children()[0]));
                        return new int(*c);
                    }
                    else
                    {
                        std::cerr << *tree << std::endl << "Is of type int_t"
                            << std::endl;
                        exit(-1);
                    }
                case types::str_t:
                    if(function == func::nil)
                    {
                        return new std::string("");
                    }
                    else if(function == func::cons)
                    {
                        auto c = std::unique_ptr<char>(
                                (char*)Symbol::evaluate(tree->get_children()[0]));
                        auto str = std::unique_ptr<std::string>(
                                (std::string*)
                                Symbol::evaluate(tree->get_children()[1]));
                        return new std::string(*c + *str);
                    }
                    else if(function == func::tl)
                    {
                        auto str = std::unique_ptr<std::string>(
                                (std::string*)
                                Symbol::evaluate(tree->get_children()[0]));
                        if(str->length() > 0)
                            return new std::string(str->c_str() + 1);
                        else
                            return new std::string("");
                    }
                    else
                    {
                        std::cerr << *tree << std::endl << "Is of type str_t"
                            << std::endl;
                        exit(-1);
                    }
                    break;
                default:
                    break;
            }
        }

        char get_c_value() const { return c_value; }
        int  get_i_value() const { return i_value; }

        friend std::ostream & operator<<(std::ostream &os, const Symbol &sym)
        {
            switch(sym.type)
            {
                case func::nil:
                    os << "nil";
                    break;
                case func::cons:
                    os << "cons";
                    break;
                case func::tl:
                    os << "tl";
                    break;
                case func::int_const:
                    os << sym.i_value;
                    break;
                case func::plus:
                    os << "plus";
                    break;
                case func::minus:
                    os << "minus";
                    break;
                case func::ord:
                    os << "ord";
                    break;
                case func::char_const:
                    os << sym.c_value;
                    break;
                case func::hd:
                    os << "hd";
                    break;
                default:
                    break;
            }
            return os;
        }
    private:
        func type;
        int i_value;
        char c_value;
};

tree_ptr<Symbol> random_tree();

double fitness(tree_ptr<Symbol> tree);
