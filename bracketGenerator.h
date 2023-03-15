#pragma once
#include "bracket.h"
#include <unordered_map>

class BracketGenerator {
private:
    Bracket* base = nullptr;
    Bracket* bracket = nullptr;
    std::string year;
    double teamScores[70];

public:
    int getScoreFromStatWeights(const std::vector<double>& statWeights) {
        for (int i = 0; i < 64; i++) {
            int id = base->field[i].id;
            teamScores[id] = 0;
            for (int j = 0; j < (int)statWeights.size(); j++)
                teamScores[id] += statWeights[j] * std::stod(DataStore::getTeamData(year, id)[j + 5]);
        }

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < std::pow(2.0, 5 - i); j++) {
                auto matchup = bracket->getRoundMatchup(i, j);
                bracket->makeChoice(i, j, teamScores[matchup.first->id] > teamScores[matchup.second->id] ? 0 : 1);
            }
        }
        return bracket->getBracketScore(base);
    }

    BracketGenerator(std::string y) {
        base = new Bracket(y, true);
        bracket = new Bracket(y, false);
        year = y;
    }

    BracketGenerator(const BracketGenerator& b) {
        base = new Bracket(b.year, true);
        bracket = new Bracket(b.year, false);
        year = b.year;
        for (int i = 0; i < 70; i++) teamScores[i] = b.teamScores[i];
    }

    ~BracketGenerator() {
        if (base != nullptr)
            delete base;
        if (bracket != nullptr)
            delete bracket;
    }
};