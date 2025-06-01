#pragma once
#include <string>
#include <iostream>
#include "Position.hpp"
#include <stdexcept>

using namespace std;
// const long long SALARY_MAX = 500000000;
const int NUMBER_MIN = 1;
const int NUMBER_MAX = 99;

enum class Position {
    CF, LWF, SS, AM, RWF,
    LM, CM, RM,
    LWB, DM, RWB,
    LB, CB, RB, GK,
    INVALID
};

class Player 
{
    public:
        Player(const std::string& name, int number, long long salary, const std::string& position, int teamId);
        void print() const;
        string getName() const;
        int getNumber() const;
        long long getSalary() const;
        string getPosition() const;
        int getTeamId() const;

        void setId(int id);
        
    private:
        int id;
        string name;
        int number;
        long long salary;
        string position;
        int teamId;
};
