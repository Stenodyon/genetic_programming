#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <vector>

typedef std::list<unsigned int> pos;

/** A tree holding values of type T
 *
 * This class represents a tree (as in a graph without cycles) with
 * values of type T attached to the nodes. */
template<typename T>
class Tree
{
    private:
        /// The value of type T attached to that node.
        std::shared_ptr<T> node;
        /// The array of children of that node.
        std::vector<std::shared_ptr<Tree<T>>> children;

        /** Depth-first visitor for a tree
         *
         * This nested class represents a visitor, visiting the nodes of a
         * tree depth-first and applying the provided void(pos) function
         * to each node.
         *
         * TODO maybe change the function from void(pos)
         * to void(tree_ptr<T>,pos) ? */
        class visitor
        {
            private:
                /// The root of the visited tree
                Tree<T> * root;
                /// The function applied to each node
                std::function<void(pos)> visit;

                /** This function recursively applies visit(pos) to the tree
                 * \param node The next node to be visited and whose children
                 * will be visited next
                 * \param current_position The position of node in the tree */
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
                /** Constructor for Tree<T>::visitor
                 * \param root The root node of the tree to visit
                 * \param visit The function which will be applied to the nodes
                 */
                visitor(Tree<T> * root, std::function<void(pos)> visit)
                    : root(root), visit(visit) {}

                /** This function runs the visitor and applies visit(pos) to
                 * each node */
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
        /** Copy constructor */
        Tree(const Tree<T> &copy)
            : node(std::make_shared<T>(*(copy.node)))
        {
            for(auto child : copy.children)
            {
                auto child_copy = std::make_shared<Tree<T>>(*child);
                children.push_back(child_copy);
            }
        }

        /** Getter for the value of type T attached to the node
         * \return A pointer to the value */
        const std::shared_ptr<T> & get_node() const { return node; }
        /** Getter for children of that node
         * \return A reference to the array of children */
        const std::vector<std::shared_ptr<Tree<T>>> & get_children() const
        { return children; }
        /** Getter for a particular subtree of that tree
         * \param position The position of the subtree to return within that
         * tree
         * \return A pointer to the corresponding subtree
         *
         * TODO behavior when the position is outside of the bounds */
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

        /** This function applies visit_func to all nodes depth-first
         * \param visit_func The function to apply to each node */
        void visit(std::function<void(pos)> visit_func)
        { visitor(this, visit_func).accept(); }

        /** This function replaces the subtree at a given position by
         * another tree
         * \param newtree The tree that will replace the subtree
         * \param position The positon at which the replacement will
         * be made */
        void replace(std::shared_ptr<Tree<T>> &newtree, pos &position);

        /** This function returns a uniformly distributed random position
         * within the tree */
        pos random_position();

        /** Output function */
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
