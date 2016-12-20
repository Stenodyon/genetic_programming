#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <vector>

typedef std::list<unsigned int> pos;

template<typename T>
class Tree
{
    private:
        std::shared_ptr<T> node; // T value of that node
        std::vector<std::shared_ptr<Tree<T>>> children; // Array of children

        class visitor
        {
            private:
                Tree<T> * root;
                std::function<void(pos)> visit;

                void accept(
                        Tree<T> * node,
                        pos &current_position)
                {
                    visit(current_position);
                    for(unsigned int i = 0; i < node->children.size(); i++)
                    {
                        current_position.push_back(i);
                        accept(node->children[i].get(), current_position);
                        current_position.pop_back();
                    }
                }
            public:
                visitor(Tree<T> * root, std::function<void(pos)> visit)
                    : root(root), visit(visit) {}

                void accept()
                {
                    pos p = {};
                    accept(root, p);
                }
        };

    public:
        Tree() {}
        Tree(std::shared_ptr<T> node) : node(node) {}
        Tree(std::shared_ptr<T> node,
                std::vector<std::shared_ptr<Tree<T>>> children)
            : node(node), children(children) {}
        Tree(const Tree<T> &copy)
            : node(std::make_shared<T>(*(copy.node)))
        {
            for(auto child : copy.children)
            {
                auto child_copy = std::make_shared<Tree<T>>(*child);
                children.push_back(child_copy);
            }
        }

        const std::shared_ptr<T> & get_node() const { return node; }
        const std::vector<std::shared_ptr<Tree<T>>> & get_children() const
        { return children; }
        std::shared_ptr<Tree<T>> get_subtree(pos &position)
        {
            unsigned int i = position.front();
            if(position.size() == 1)
            {
                return children[i];
            }
            else
            {
                position.pop_front();
                return children[i]->get_subtree(position);
            }
        }

        void visit(std::function<void(pos)> visit_func)
        { visitor(this, visit_func).accept(); }

        void replace(std::shared_ptr<Tree<T>> &newtree, pos &position);

        pos random_position();

        template<typename U>
        friend std::ostream & operator<<(std::ostream & os,
                const Tree<U> & tree);
};


template<typename T>
void Tree<T>::replace(
        std::shared_ptr<Tree<T>> &newtree,
        std::list<unsigned int> &position)
{
    unsigned int i = position.front();
    if(position.size() == 1)
    {
        children[i] = std::make_shared<Tree<T>>(*newtree);
    }
    else
    {
        position.pop_front();
        children[i]->replace(newtree, position);
    }
}

template<typename T>
class reservoir_sampler
{
    private:
        unsigned int i = 1;
        T value;
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<> dis;
    public:
        reservoir_sampler() : gen(rd()), dis(0,1) {}
        void operator()(T val)
        {
            if(dis(gen) < (1.0 / (double)i))
                this->value = val;
            i++;
        }
        T get_value() const { return value; }
};

template<typename T>
pos Tree<T>::random_position()
{
    reservoir_sampler<pos> rs;
    visit(std::ref(rs));
    return rs.get_value();
}

template<typename T>
std::ostream & operator<<(std::ostream & os, const Tree<T> & tree)
{
    os << *(tree.node) << "(";
    for(auto child : tree.children)
        os << *child << ",";
    os << ")";
    return os;
}
