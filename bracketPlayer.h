#include "csvParser.h"
#include <fstream>

class BracketPlayer {


    BracketPlayer(std::string year) {
        std::ifstream file("data/2018.csv");
        auto bracketYear = readCSV(file);
        file.close();
    }
};