#include "Team.hpp"

Team::Team(const std::string& name, long long salaryCap)
{
    if (salaryCap < 0 || salaryCap > SALARY_MAX)
        throw invalid_argument("Salary cap must be between 0 and 5,000,000,000");

    this->name = name;
    this->salaryCap = salaryCap;
}

void Team::print() const
{
    std::cout << "Team: " << name << " | Salary Cap: " << salaryCap << '\n';
}

string Team::getName() const
{
    return this->name;
}   

long long Team::getSalaryCap() const
{
    return this->salaryCap;
}

//ok