#include <iostream>
#include <random>
#include <chrono> 
#include "dataStore.h"
#include "statWeightRunner.h"

std::default_random_engine e;
std::uniform_real_distribution<> dis(0, 1); // rage 0 - 1
std::student_t_distribution<> stdis{200.0f};

float rand_weight() {
    return dis(e);
}

float fuzz_weight(double weight, double muteFactor) {
    return std::min(1.0, std::max(0.0, weight + (stdis(e) / muteFactor)));
}

void printBests(int bestScore, std::vector<double> bestWeights) {
    std::cout << "BEST AVG SCORE: " << bestScore / (double)NUM_YEARS << std::endl;
    for (int i = 0; i < (int)bestWeights.size(); i++) {
        std::printf("%-10f%s\n", bestWeights[i], DataStore::statNames[i].c_str());
    }
}

int main() {
    e.seed(std::chrono::system_clock::now().time_since_epoch().count());
    DataStore::init();
    std::cout << "HELLO WORLD!" << std::endl;

    StatWeightRunner runner;

    int bestScore = 0;
    std::vector<double> bestWeights;

    for (int i = 0; i < 2000; i++) {
        std::vector<double> weights; //{1.0, 0.0, 0.0};
        for (int j = 0; j < NUM_STATS; j++) weights.push_back(rand_weight());
        double score = runner.runWeights(weights, bestScore);
        if (score > bestScore) {
            bestScore = score;
            bestWeights = weights;
        }
    }

    printBests(bestScore, bestWeights);

    std::cout << "Now let's do some weight fuzzing..." << std::endl;
    for (int i = 0; i < 2000; i++) {
        std::vector<double> testWeights = bestWeights;
        for (int j = 0; j < (int)testWeights.size(); j++) testWeights[j] = fuzz_weight(testWeights[j], i < 1000 ? 4.0 : 5.0);
        double score = runner.runWeights(testWeights, bestScore);
        if (score > bestScore) {
            bestScore = score;
            bestWeights = testWeights;
        }
    }

    printBests(bestScore, bestWeights);

    bool foundImprovement;
    do {
        foundImprovement = false;
        std::cout << "Now adjusting individual weights to full range to see if improvement" << std::endl;

        // We want to see which stat change invokes the biggest score improvement
        double bestImprovementScore = bestScore;
        int bestImprovementWeightIndex = 0;
        double bestImprovementNewWeight = 0.0;
        for (int i = 0; i < (int)bestWeights.size(); i++) {
            double oldWeight = bestWeights[i];
            for (int j = 0; j <= 50; j++) {
                bestWeights[i] = j / 25.0; // Allowing weights to run up to 2 instead of just 1.
                double newScore = runner.runWeights(bestWeights, bestScore);
                if (newScore > bestImprovementScore) {
                    bestImprovementScore = newScore;
                    bestImprovementNewWeight = bestWeights[i];
                    bestImprovementWeightIndex = i;
                    foundImprovement = true;
                }
            }
            bestWeights[i] = oldWeight;
        }
        if (foundImprovement) {
            bestScore = bestImprovementScore;
            bestWeights[bestImprovementWeightIndex] = bestImprovementNewWeight;
        }
    } while (foundImprovement);

    printBests(bestScore, bestWeights);

    std::cout << "Done.";
}