#include <iostream>
#include <thread>
#include <future>
#include "weightFinderWorker.h"

const int NUM_THREADS = 8;

void printBests(WeightResults bests) {
    std::cout << "BEST AVG SCORE: " << bests.bestScore / (double)NUM_YEARS << std::endl;
    for (int i = 0; i < (int)bests.bestWeights.size(); i++) {
        std::printf("%-10f%s\n", bests.bestWeights[i], DataStore::statNames[i].c_str());
    }
}

int main() {
    DataStore::init();
    std::cout << "HELLO WORLD!" << std::endl;

    std::vector<WeightFinderWorker> workers;
    workers.resize(NUM_THREADS);

    std::vector<std::future<WeightResults>> threads;
    WeightResults bestResults;
    for (WeightFinderWorker& worker : workers) {
        threads.push_back(std::async(&WeightFinderWorker::findBestScoreByRandomizingWeights, &worker, RandomizeMethod::COMPLETE_RANDOM, 1000, bestResults, 0.0));
    }
    for (auto& thread : threads) {
        WeightResults results = thread.get();
        if (results.bestScore > bestResults.bestScore) bestResults = results;
    }

    printBests(bestResults);

    std::cout << "Lets do some fuzzing" << std::endl;
    threads.clear();
    for (WeightFinderWorker& worker : workers) {
        threads.push_back(std::async(&WeightFinderWorker::findBestScoreByRandomizingWeights, &worker, RandomizeMethod::DISTRIBUTED_FUZZ, 1000, bestResults, 4.0));
    }
    for (auto& thread : threads) {
        WeightResults results = thread.get();
        if (results.bestScore > bestResults.bestScore) bestResults = results;
    }

    printBests(bestResults);

    bool foundImprovement;
    workers.resize(NUM_STATS);
    do {
        threads.clear();
        foundImprovement = false;
        std::cout << "Now adjusting individual weights to full range to see if improvement" << std::endl;
        int i = 0;
        for (WeightFinderWorker& worker : workers) {
            threads.push_back(std::async(&WeightFinderWorker::findBestSingleWeight, &worker, bestResults, i));
            i += 1;
        }
        for (auto& thread : threads) {
            WeightResults results = thread.get();
            if (results.bestScore > bestResults.bestScore) {
                bestResults = results;
                foundImprovement = true;
            }
        }
    } while (foundImprovement);

    printBests(bestResults);

    std::cout << "Done.";
}