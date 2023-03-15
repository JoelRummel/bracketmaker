#include <random>
#include "statWeightRunner.h"

struct WeightResults {
    std::vector<double> bestWeights;
    double bestScore;
};

enum RandomizeMethod {
    COMPLETE_RANDOM,
    DISTRIBUTED_FUZZ
};

class WeightFinderWorker {
private:
    std::default_random_engine e;
    std::uniform_real_distribution<> dis{0, 1};
    std::student_t_distribution<> stdis{200.0f};
    StatWeightRunner runner;

    float rand_weight() {
        return dis(e);
    }

    float fuzz_weight(double weight, double muteFactor) {
        return std::min(1.0, std::max(0.0, weight + (stdis(e) / muteFactor)));
    }

public:
    WeightResults findBestScoreByRandomizingWeights(RandomizeMethod method, int iterations, WeightResults presetResults = {}, double randArg = 1.0) {
        WeightResults results;
        if (method == RandomizeMethod::DISTRIBUTED_FUZZ) {
            results = presetResults;
        }

        for (int i = 0; i < iterations; i++) {
            std::vector<double> testWeights;
            if (method == RandomizeMethod::DISTRIBUTED_FUZZ) {
                testWeights = results.bestWeights;
                for (int j = 0; j < (int)testWeights.size(); j++) {
                    testWeights[j] = fuzz_weight(testWeights[j], randArg);
                }
            } else {
                for (int j = 0; j < NUM_STATS; j++) {
                    testWeights.push_back(rand_weight());
                }
            }
            double score = runner.runWeights(testWeights, results.bestScore);
            if (score > results.bestScore) {
                results.bestScore = score;
                results.bestWeights = testWeights;
            }
        }

        return results;
    }

    WeightResults findBestSingleWeight(WeightResults existingResults, int weightIndex) {
        WeightResults bestResults = existingResults;
        double bestNewWeight = existingResults.bestWeights[weightIndex];

        for (int j = 0; j <= 50; j++) {
            bestResults.bestWeights[weightIndex] = j / 25.0; // Allowing weights to run up to 2 instead of just 1.
            double newScore = runner.runWeights(bestResults.bestWeights, bestResults.bestScore);
            if (newScore > bestResults.bestScore) {
                bestResults.bestScore = newScore;
                bestNewWeight = bestResults.bestWeights[weightIndex];
            }
        }

        bestResults.bestWeights[weightIndex] = bestNewWeight;
        return bestResults;
    }

    WeightFinderWorker() {
        e.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }
};