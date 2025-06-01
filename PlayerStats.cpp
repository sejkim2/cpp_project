#include "PlayerStats.hpp"
#include <stdexcept>

PlayerStats::PlayerStats(int playerId, int attack, int defend, int speed, int strength, int shoot, int pass)
{
  validateStats({attack, defend, speed, strength, shoot, pass});
  
  this->playerId = playerId;
  this->attack = attack;
  this->defend = defend;
  this->speed = speed;
  this->strength = strength;
  this->shoot = shoot;
  this->pass = pass;
}

void PlayerStats::print() const {
    cout << " Stats - "
              << "Attack: " << attack
              << ", Defend: " << defend
              << ", Speed: " << speed
              << ", Strength: " << strength
              << ", Shoot: " << shoot
              << ", Pass: " << pass
              << '\n';
}

void PlayerStats::validateStats(const vector<int>& stats)
{
  for (int val : stats) {
        if (val < MIN_STAT || val > MAX_STAT) {
            throw invalid_argument("All stat values must be between 0 and 10.");
        }
    }
}