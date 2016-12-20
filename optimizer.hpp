#pragma once

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
                population.push_back(rand_individual());
        }

        void compute_scores(std::list<tree_ptr<T>> &population,
                            std::vector<double>    &scores)
        {
            std::vector<double> new_scores;
            for(tree_ptr<T> tree : population)
                new_scores.push_back(eval_fitness(tree));
            scores = new_scores;
        }

        void natural_selection(std::list<tree_ptr<T>> &population,
                               std::vector<double>    &scores)
        {
            double total_score = std::accumulate(scores.begin(),
                                                 scores.end(),
                                                 0);
            std::list<tree_ptr<T>> new_population;
            unsigned int i = 0;
            for(tree_ptr<T> tree : population)
            {
                double probability = scores[i] / total_score;
                if(dis(gen) >= probability)
                    new_population.push_back(tree);
                i++;
            }
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
                _cross_over(population);
        }

        void step(std::list<tree_ptr<T>> &population,
                  std::vector<double>    &scores)
        {
            populate(population);
            compute_scores(population, scores);
            natural_selection(population, scores);
            cross_over(population);
        }

    public:
        Optimizer(std::function<double(tree_ptr<T>)> eval_fitness,
                  std::function<tree_ptr<T>(void)> rand_individual,
                  unsigned int max_population = 100)
            : eval_fitness(eval_fitness), rand_individual(rand_individual),
            max_population(max_population),
            gen(rd()), dis(0,1)
        {}

        void run()
        {
            std::list<tree_ptr<T>> population;
            std::vector<double>    scores;
            for(unsigned int i = 0; i < 2; i++)
            {
                std::cout << "STEP " << i << std::endl;
                step(population, scores);
            }
        }
};


