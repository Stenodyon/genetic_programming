#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <tuple>
#include <vector>

typedef std::list<unsigned int> pos;

/** A tree holding values of type T
 *
 * This class represents a tree (as in a graph without cycles) with
 * values of type T attached to the nodes. */
template<typename T, typename node_type_t>
class Tree
{
    private:
        /// The value of type T attached to that node.
        T node;
        /// The type of the node
        node_type_t type;
        /// The array of children of that node.
        std::vector<std::shared_ptr<Tree<T,node_type_t>>> children;

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
                Tree<T,node_type_t>* root;
                /// The function applied to each node
                std::function<void(Tree<T,node_type_t>*,pos)> visit;

                /** This function recursively applies visit(pos) to the tree
                 * \param node The next node to be visited and whose children
                 * will be visited next
                 * \param current_position The position of node in the tree */
                void accept(
                        Tree<T,node_type_t>* node,
                        pos &current_position)
                {
                    visit(node, current_position);
                    for(unsigned int i = 0; i < node->children.size(); i++)
                    {
                        current_position.push_back(i);
                        accept(node->children[i].get(), current_position);
                        current_position.pop_back();
                    }
                }
            public:
                /** Constructor for Tree<T,node_type_t>::visitor
                 * \param root The root node of the tree to visit
                 * \param visit The function which will be applied to the nodes
                 */
                visitor(Tree<T,node_type_t>* root,
                        std::function<void(Tree<T,node_type_t>*,pos)> visit)
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
        Tree(T node, node_type_t type) : node(node), type(type) {}
        Tree(T node, node_type_t type,
             std::vector<std::shared_ptr<Tree<T,node_type_t>>> children)
            : node(node), type(type), children(children) {}
        /** Copy constructor */
        Tree(const Tree<T,node_type_t> &copy)
            : node(T(copy.node)), type(copy.type)
        {
            for(auto child : copy.children)
            {
                auto child_copy = std::make_shared<Tree<T,node_type_t>>(*child);
                children.push_back(child_copy);
            }
        }

        /** Adds given children to the children of the tree
         * \param child The child to add */
        void add(std::shared_ptr<Tree<T,node_type_t>> child)
        { children.push_back(child); }

        /** Getter for the value of type T attached to the node
         * \return A reference to the held value */
        T & get_node() { return node; }
        /** Const getter for the value of type T attached to the node
         * \return A const reference to the held value */
        const T & get_node() const { return node; }
        /** Getter for the node type
         * \return The type of the node */
        node_type_t get_type() const { return type; }
        /** Getter for children of that node
         * \return A reference to the array of children */
        const std::vector<std::shared_ptr<Tree<T,node_type_t>>> & get_children() const
        { return children; }
        /** Getter for a particular subtree of that tree
         * \param position The position of the subtree to return within that
         * tree
         * \return A pointer to the corresponding subtree
         *
         * TODO behavior when the position is outside of the bounds */
        std::shared_ptr<Tree<T,node_type_t>> get_subtree(pos &position)
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
        void visit(std::function<void(Tree<T,node_type_t>*,pos)> visit_func)
        { visitor(this, visit_func).accept(); }

        /** This function replaces the subtree at a given position by
         * another tree
         * \param newtree The tree that will replace the subtree
         * \param position The positon at which the replacement will
         * be made */
        void replace(std::shared_ptr<Tree<T,node_type_t>> &newtree, pos &position);

        /** This function returns a uniformly distributed random position
         * whose subtree is of given type  within the tree
         * \param type The type of the subtree to get
         * \return A tuple (has_found,position) */
        std::pair<bool,pos> random_position(node_type_t type);

        /** This function returns a uniformly distributed random position
         * within the tree
         * \return The position */
        pos random_position();

        /** Output function */
        template<typename U, typename Unode_type_t>
        friend std::ostream & operator<<(std::ostream & os,
                const Tree<U, Unode_type_t> & tree);
};

template<typename T, typename node_type_t>
void Tree<T,node_type_t>::replace(
        std::shared_ptr<Tree<T,node_type_t>> &newtree,
        std::list<unsigned int> &position)
{
    unsigned int i = position.front();
    if(position.size() == 1)
    {
        children[i] = std::make_shared<Tree<T,node_type_t>>(*newtree);
    }
    else
    {
        position.pop_front();
        children[i]->replace(newtree, position);
    }
}

template<typename T, typename node_type_t>
class reservoir_sampler
{
    private:
        unsigned int i = 1;
        Tree<T,node_type_t>* tree;
        pos position;
        std::function<bool(Tree<T,node_type_t>*)> filter;
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<> dis;
    public:
        reservoir_sampler(
                std::function<bool(Tree<T,node_type_t>*)> filter)
            : filter(filter), gen(rd()), dis(0,1) {}
        void operator()(Tree<T,node_type_t>* subtree, pos position)
        {
            if(filter(subtree))
            {
                if(dis(gen) < (1.0 / (double)i))
                {
                    this->tree = subtree;
                    this->position = position;
                }
                i++;
            }
        }
        Tree<T,node_type_t>* get_tree() const
        { return tree; }
        pos get_position() const { return position; }
        const unsigned int get_iterations() const { return i; }
};

template<typename T, typename node_type_t>
bool any_filter(Tree<T,node_type_t>* tree)
{ return true; }

template<typename T, typename node_type_t>
struct type_filter
{
    private:
        node_type_t type;
    public:
        type_filter(node_type_t type) : type(type) {}
        bool operator()(Tree<T,node_type_t>* tree)
        { return tree->get_type() == type; }
};

template<typename T, typename node_type_t>
pos Tree<T,node_type_t>::random_position()
{
    reservoir_sampler<T, node_type_t> rs{&any_filter<T,node_type_t>};
    visit(std::ref(rs));
    return rs.get_position();
}

template<typename T, typename node_type_t>
std::pair<bool,pos> Tree<T,node_type_t>::random_position(node_type_t type)
{
    reservoir_sampler<T, node_type_t> rs{type_filter<T,node_type_t>(type)};
    visit(std::ref(rs));
    return std::pair<bool,pos>(rs.get_iterations() == 0, rs.get_position());
}

template<typename T, typename node_type_t>
std::ostream & operator<<(std::ostream & os, const Tree<T,node_type_t> & tree)
{
    os << tree.node << "(";
    for(auto child : tree.children)
        os << *child << ",";
    os << ")";
    return os;
}
