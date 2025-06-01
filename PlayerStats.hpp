#pragma once

#include <iostream>
#include <vector>
using namespace std;

const int MIN_STAT = 0;
const int MAX_STAT = 10;

class PlayerStats {
public:
    int playerId;
    int attack, defend, speed, strength, shoot, pass;

    PlayerStats() = default;
    PlayerStats(int playerId, int attack, int defend, int speed,
                int strength, int shoot, int pass);
    void print() const;

    private:
        void validateStats(const vector<int>& stats);
};
