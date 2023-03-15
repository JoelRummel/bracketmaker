#include "bracketGenerator.h"

const int NUM_STATS = 23;

const int NUM_YEARS = 12;

class StatWeightRunner {
private:
    std::vector<BracketGenerator> brackets;

public:
    double runWeights(const std::vector<double>& weights, double currentBestScore = 0.0) {
        double score = 0.0;
        int bracketsCounted = 0;

        for (auto b : brackets) {
            score += b.getScoreFromStatWeights(weights);
            bracketsCounted += 1;
            // Optimization: if first few brackets are performing exceptionally poorly, cut loose
            double minScoreRatio = std::max(0.3, 0.8 * (bracketsCounted / NUM_YEARS)); // More tolerant if fewer brackets counted
            if (score / (double)bracketsCounted < (currentBestScore / NUM_YEARS) * minScoreRatio) break;
        }
        return score;
    }

    StatWeightRunner() {
        for (auto year : years) {
            brackets.emplace_back(year);
        }
    }
};