#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <numeric>
#include <random>
#include <vector>

#include "tree.hpp"

template<typename T>
using tree_ptr = std::shared_ptr<Tree<T>>;

template<typename T>
class Optimizer
{
    private:
        std::function<double(tree_ptr<T>)> eval_fitness;
        std::function<tree_ptr<T>(void)> rand_individual;

        const unsigned int max_population;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<> dis;

        void populate(std::list<tree_ptr<T>> & population)
        {
            while(population.size() < max_population)
            {
                std::cout << "|" << std::flush;
                population.push_back(rand_individual());
            }
            std::cout << std::endl << population.size() << " trees"
                << std::endl;
        }

        void compute_scores(std::list<tree_ptr<T>> &population,
                            std::vector<double>    &scores)
        {
            std::vector<double> new_scores;
            for(tree_ptr<T> tree : population)
            {
                std::cout << "|" << std::flush;
                new_scores.push_back(eval_fitness(tree));
            }
            std::cout << std::endl;
            scores = new_scores;
        }

        void natural_selection(std::list<tree_ptr<T>> &population,
                               std::vector<double>    &scores)
        {
            double max_score = *std::max_element(scores.begin(),
                                                scores.end());
            std::list<tree_ptr<T>> new_population;
            std::cout << scores.size() << std::endl;
            while(new_population.size() == 0) // In case all population dies
            {
                unsigned int i = 0;
                std::cout << "|" << std::flush;
                for(tree_ptr<T> tree : population)
                {
                    double probability = (scores[i] + 1) / (max_score + 1);
                    if(dis(gen) < probability)
                        new_population.push_back(tree);
                    i++;
                }
            }
            std::cout << std::endl << new_population.size() << " trees kept"
                << std::endl;
            population = new_population;
        }

        void _cross_over(std::list<tree_ptr<T>> &population)
        {
            unsigned int n = population.size();
            unsigned int ind1 = (unsigned int)(dis(gen) * n);
            unsigned int ind2 = (unsigned int)(dis(gen) * n);
            tree_ptr<T> tree1 = *std::next(population.begin(), ind1);
            tree_ptr<T> tree2 = *std::next(population.begin(), ind2);
            pos pos1 = tree1->random_position();
            pos pos2 = tree2->random_position();
            tree_ptr<T> subtree1, subtree2;
            if(pos1.size() == 0)
            {
                if(pos2.size() == 0)
                    return;
                subtree2 = tree2->get_subtree(pos2);
                population.push_back(subtree2);
                tree2->replace(tree1, pos2);
            }
            else
            {
                subtree1 = tree1->get_subtree(pos1);
                if(pos2.size() == 0)
                {
                    population.push_back(subtree1);
                    tree1->replace(tree2, pos1);
                    return;
                }
                subtree2 = tree2->get_subtree(pos2);
                tree1->replace(subtree2, pos1);
                tree2->replace(subtree1, pos2);
            }
        }

        void cross_over(std::list<tree_ptr<T>> &population)
        {
            for(unsigned int i = 0; i < 20; i++)
            {
                std::cout << "|" << std::flush;
                _cross_over(population);
            }
            std::cout << std::endl;
        }

        void step(std::list<tree_ptr<T>> &population,
                  std::vector<double>    &scores)
        {
            std::cout << "populating..." << std::endl;
            populate(population);
            std::cout << "computing scores..." << std::endl;
            compute_scores(population, scores);
            std::cout << "naturally selecting..." << std::endl;
            natural_selection(population, scores);
            std::cout << "crossing over..." << std::endl;
            cross_over(population);
            std::cout << std::endl;
        }

        tree_ptr<T> get_best(std::list<tree_ptr<T>> &population,
                             std::vector<double>    &scores)
        {
            auto max_score = std::max_element(scores.begin(),
                                              scores.end());
            unsigned int index = std::distance(scores.begin(), max_score);
            return *std::next(population.begin(), index);
        }

    public:
        Optimizer(std::function<double(tree_ptr<T>)> eval_fitness,
                  std::function<tree_ptr<T>(void)> rand_individual,
                  unsigned int max_population = 100)
            : eval_fitness(eval_fitness), rand_individual(rand_individual),
            max_population(max_population),
            gen(rd()), dis(0,1)
        {}

        tree_ptr<T> run(unsigned int steps = 10)
        {
            std::list<tree_ptr<T>> population;
            std::vector<double>    scores;
            for(unsigned int i = 0; i < (steps - 1); i++)
            {
                std::cout << "STEP " << i + 1 << std::endl;
                step(population, scores);
            }
            std::cout << "STEP " << steps << std::endl;
            populate(population);
            compute_scores(population, scores);
            return get_best(population, scores);
        }
};


