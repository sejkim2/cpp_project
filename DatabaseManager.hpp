#pragma once
#include "Player.hpp"
#include "Team.hpp"
#include "PlayerStats.hpp"
#include <vector>
#include <string>
#include <sqlite3.h>
#include <optional>
#include <fstream>
#include <sstream>
#include <iostream>

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbFile);
    ~DatabaseManager();

    void initSchema();
    void initSchemaFromFile(const std::string& filename);

    // Team
    bool addTeam(const Team& team);
    std::vector<Team> getAllTeams();
    int getTotalSalaryOfTeam(int teamId);

    // Player
    bool addPlayer(const Player& player, const PlayerStats& stats);
    std::vector<Player> getPlayersByTeam(int teamId);

    //delete
    bool deletePlayer(int playerId);
    bool deleteTeam(int teamId);

    //checkUniqueTeam
    bool isTeamNameTaken(const std::string& name);

    //find team id by name
    std::optional<int> getTeamIdByName(const std::string& name);

    //find player id by name
    std::optional<int> getPlayerIdByName(const std::string& name);

    //reset database
    void resetDatabase();

    void executeSqlFile(const string& filename);

    long long getTotalSalaryByTeam(int teamId);
    long long getSalaryCapByTeamId(int teamId);

    bool printAverageStatsByTeam(int teamId);
    bool printPlayerByName(const std::string& name);

private:
    sqlite3* db;
};
