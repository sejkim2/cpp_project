#pragma once
#include <string>
#include <set>
using namespace std;

const set<string> VALID_POSITIONS = {
    "CF", "LW", "SS", "AM", "RW",
    "LM", "CM", "RM","DM",
    "LB", "CB", "RB", "GK"
};

inline bool isValidPosition(const string& pos) {
    return VALID_POSITIONS.find(pos) != VALID_POSITIONS.end();
}
