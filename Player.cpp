#include "Player.hpp"
#include "Team.hpp"

Player::Player(const std::string &name, int number, long long salary, const std::string &position, int teamId)
{
    if (number < NUMBER_MIN || number > NUMBER_MAX)
      throw invalid_argument("Number must be between 1 and 99");
    if (salary < 0 || salary > SALARY_MAX)
      throw invalid_argument("Salary must be between 0 and 5,000,000,00");
    if (!isValidPosition(position))
      throw invalid_argument("Position must be one of the following: CF, LWF, SS, AM, RWF, LM, CM, RM, LWB, DM, RWB, LB, CB, RB, or GK.");

    this->name = name;
    this->number = number;
    this->salary = salary;
    this->position = position;
    this->teamId = teamId;
}

void Player::print() const
{
  std::cout << "Player: " << name << " | Number: " << number
            << " | Position: " << position << " | Salary: " << salary << '\n';
}

string Player::getName() const
{
  return this->name;
}

int Player::getNumber() const
{
    return this->number;
}

long long Player::getSalary() const
{
    return this->salary;
}

string Player::getPosition() const
{
    return this->position;
}

int Player::getTeamId() const
{
    return this->teamId;
}

void Player::setId(int id)
{
  this->id = id;
}