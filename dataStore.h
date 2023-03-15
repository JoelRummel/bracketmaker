#pragma once
#include "csvParser.h"
#include <fstream>
#include <unordered_map>

const std::string years[] = { "2010", "2011", "2012", "2013", "2014", "2015", "2016", "2017", "2018", "2019", "2021", "2022", };

class DataStore {
public:
    static std::unordered_map<std::string, std::vector<std::vector<std::string>>> data; // Maps year -> team -> data
    static std::vector<std::string> statNames;

    static std::vector<std::string> getTeamData(std::string year, int teamId) {
        return DataStore::data[year][teamId];
    }

    static void init() {
        for (auto year : years) {
            std::ifstream file("data/" + year + ".csv");
            auto bracketYear = readCSV(file);
            file.close();

            DataStore::data[year];
            DataStore::data[year].resize(70);
            for (auto row : bracketYear) {
                if (row[0] == "Rank") {
                    if (statNames.size() == 0) {
                        for (int i = 5; i < row.size(); i++) statNames.push_back(row[i]);
                    }
                    continue;
                }
                DataStore::data[year][std::stoi(row[0])] = row;
            }
        }
    }
};

std::unordered_map<std::string, std::vector<std::vector<std::string>>> DataStore::data;
std::vector<std::string> DataStore::statNames;