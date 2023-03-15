#pragma once
#include "dataStore.h"
#include <fstream>
#include <cmath>

const int REGION_INDEX = 3;
const int SEED_INDEX = 4;
const int SCHOOL_INDEX = 1;
const int GAMES_WON_INDEX = 2;

struct School {
    std::string school;
    int seed;
    int id;
};

const int SEED_POSITION[] = {
    //  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16
        0,14, 10,6, 4, 8, 12,2, 3, 13,9, 5, 7, 11,15,1
};

class Bracket {
public:
    School field[64];
    School* secondRound[32];
    School* thirdRound[16];
    School* quarterfinals[8];
    School* semifinals[4];
    School* finals[2];
    School* champion;

    std::pair<School*, School*> getRoundMatchup(int roundNumber, int slotNumber) {
        std::pair<School*, School*> matchup(nullptr, nullptr);
        if (roundNumber == 0) {
            matchup.first = &(field[slotNumber * 2]);
            matchup.second = &(field[slotNumber * 2 + 1]);
        }
        if (roundNumber == 1) {
            matchup.first = secondRound[slotNumber * 2];
            matchup.second = secondRound[slotNumber * 2 + 1];
        }
        if (roundNumber == 2) {
            matchup.first = thirdRound[slotNumber * 2];
            matchup.second = thirdRound[slotNumber * 2 + 1];
        }
        if (roundNumber == 3) {
            matchup.first = quarterfinals[slotNumber * 2];
            matchup.second = quarterfinals[slotNumber * 2 + 1];
        }
        if (roundNumber == 4) {
            matchup.first = semifinals[slotNumber * 2];
            matchup.second = semifinals[slotNumber * 2 + 1];
        }
        if (roundNumber == 5) {
            matchup.first = finals[0];
            matchup.second = finals[1];
        }
        return matchup;
    };

    void makeChoice(int roundNumber, int slotNumber, int choiceNumber) {
        if (roundNumber == 0) {
            secondRound[slotNumber] = &(field[slotNumber * 2 + choiceNumber]);
        }
        if (roundNumber == 1) {
            thirdRound[slotNumber] = secondRound[slotNumber * 2 + choiceNumber];
        }
        if (roundNumber == 2) {
            quarterfinals[slotNumber] = thirdRound[slotNumber * 2 + choiceNumber];
        }
        if (roundNumber == 3) {
            semifinals[slotNumber] = quarterfinals[slotNumber * 2 + choiceNumber];
        }
        if (roundNumber == 4) {
            finals[slotNumber] = semifinals[slotNumber * 2 + choiceNumber];
        }
        if (roundNumber == 5) {
            champion = finals[choiceNumber];
        }
    }

    int getBracketScore(Bracket* actualResults) {
        int totalScore = 0;
        for (int i = 0; i < 32; i++) {
            if (secondRound[i]->school == actualResults->secondRound[i]->school) totalScore += 1;
        }
        for (int i = 0; i < 16; i++) {
            if (thirdRound[i]->school == actualResults->thirdRound[i]->school) totalScore += 2;
        }
        for (int i = 0; i < 8; i++) {
            if (quarterfinals[i]->school == actualResults->quarterfinals[i]->school) totalScore += 4;
        }
        for (int i = 0; i < 4; i++) {
            if (semifinals[i]->school == actualResults->semifinals[i]->school) totalScore += 8;
        }
        for (int i = 0; i < 2; i++) {
            if (finals[i]->school == actualResults->finals[i]->school) totalScore += 16;
        }
        if (champion->school == actualResults->champion->school) totalScore += 32;
        return totalScore;
    }

    Bracket(std::string year, bool fillFromCsv = false) {
        for (int i = 0; i < 64; i++) {
            field[i] = School{ "", -1, -1 };
            if (i < 32) secondRound[i] = nullptr;
            if (i < 16) thirdRound[i] = nullptr;
            if (i < 8) quarterfinals[i] = nullptr;
            if (i < 4) semifinals[i] = nullptr;
            if (i < 2) finals[i] = nullptr;
        }

        for (auto& row : DataStore::data[year]) {
            if (row.size() == 0) continue;
            if (row[0] == "Rank") continue;
            int position = (std::stoi(row[REGION_INDEX]) * 16) + SEED_POSITION[std::stoi(row[SEED_INDEX]) - 1];
            field[position] = School{ row[SCHOOL_INDEX], std::stoi(row[SEED_INDEX]), std::stoi(row[0]) };
            if (fillFromCsv) {
                for (int i = 0; i < std::stoi(row[GAMES_WON_INDEX]); i++) {
                    int slotNumber = std::floor(std::floor(position / 2.0) / std::pow(2.0, i));
                    auto matchup = getRoundMatchup(i, slotNumber);
                    makeChoice(i, slotNumber, (matchup.first != nullptr && matchup.first->school == row[SCHOOL_INDEX]) ? 0 : 1);
                }
            }
        }
    }
};