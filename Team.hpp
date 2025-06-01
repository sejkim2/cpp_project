#pragma once
#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;

const long long SALARY_MAX = 5000000000LL;

class Team 
{
    public:
        Team(const std::string& name, long long salaryCap);

        string getName() const;
        long long getSalaryCap() const;
        void print() const;

    private:
        // int id;
        string name;
        long long salaryCap;
};

//ok