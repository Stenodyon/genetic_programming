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
#ifdef VERBOSE
                std::cout << "|" << std::flush;
#endif
                population.push_back(rand_individual());
            }
#ifdef VERBOSE
            std::cout << std::endl << population.size() << " trees"
                << std::endl;
#endif
        }

        void compute_scores(std::list<tree_ptr<T>> &population,
                            std::vector<double>    &scores)
        {
            std::vector<double> new_scores;
            for(tree_ptr<T> tree : population)
            {
#ifdef VERBOSE
                std::cout << "|" << std::flush;
#endif
                new_scores.push_back(eval_fitness(tree));
            }
#ifdef VERBOSE
            std::cout << std::endl;
#endif
            scores = new_scores;
        }

        void natural_selection(std::list<tree_ptr<T>> &population,
                               std::vector<double>    &scores)
        {
            double max_score = get_best_fitness(scores);
            std::list<tree_ptr<T>> new_population;
#ifdef VERBOSE
            std::cout << scores.size() << std::endl;
#endif
            while(new_population.size() == 0) // In case all population dies
            {
                unsigned int i = 0;
#ifdef VERBOSE
                std::cout << "|" << std::flush;
#endif
                for(tree_ptr<T> tree : population)
                {
                    double probability = (scores[i] + 1) / (max_score + 1);
                    if(dis(gen) < probability)
                        new_population.push_back(tree);
                    i++;
                }
            }
#ifdef VERBOSE
            std::cout << std::endl << new_population.size() << " trees kept"
                << std::endl;
#endif
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
#ifdef VERBOSE
                std::cout << "|" << std::flush;
#endif
                _cross_over(population);
            }
#ifdef VERBOSE
            std::cout << std::endl;
#endif
        }

        void step(std::list<tree_ptr<T>> &population,
                  std::vector<double>    &scores)
        {
#ifdef VERBOSE
            std::cout << "naturally selecting..." << std::endl;
#endif
            natural_selection(population, scores);
#ifdef VERBOSE
            std::cout << "crossing over..." << std::endl;
#endif
            cross_over(population);
#ifdef VERBOSE
            std::cout << "populating..." << std::endl;
#endif
            populate(population);
#ifdef VERBOSE
            std::cout << "computing scores..." << std::endl;
#endif
            compute_scores(population, scores);
#ifdef VERBOSE
            std::cout << std::endl;
#endif
        }

        tree_ptr<T> get_best(std::list<tree_ptr<T>> &population,
                             std::vector<double>    &scores)
        {
            auto max_score = std::max_element(scores.begin(),
                                              scores.end());
            unsigned int index = std::distance(scores.begin(), max_score);
            return *std::next(population.begin(), index);
        }

        double get_best_fitness(std::vector<double> &scores)
        {
            return *std::max_element(scores.begin(), scores.end());
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
            populate(population);
            compute_scores(population, scores);
            for(unsigned int i = 0; i < steps; i++)
            {
                std::cout << "\rSTEP " << i + 1 << std::flush;
                step(population, scores);
            }
            std::cout << std::endl;
            return get_best(population, scores);
        }

        tree_ptr<T> run_until_fitness(double target_fitness)
        {
            std::list<tree_ptr<T>> population;
            std::vector<double>    scores;
            populate(population);
            compute_scores(population, scores);
            unsigned int i = 0;
            while(get_best_fitness(scores) < target_fitness)
            {
                std::cout << "\rSTEP " << i + 1 << std::flush;
                step(population, scores);
                i++;
            }
            std::cout << std::endl;
            return get_best(population, scores);
        }
};


