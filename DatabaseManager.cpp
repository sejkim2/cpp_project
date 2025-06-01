#include "DatabaseManager.hpp"
#include <iostream>

//connect sqlite in constructor
DatabaseManager::DatabaseManager(const std::string& dbFile) 
{
    if (sqlite3_open(dbFile.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << '\n';
        db = nullptr;
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
}

DatabaseManager::~DatabaseManager()
{
    if (db) sqlite3_close(db);
}

//this method not use -> use initSchemaFromFile() now
void DatabaseManager::initSchema()
{
    const char* sql = R"sql(
        CREATE TABLE IF NOT EXISTS teams (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            salary_cap INTEGER NOT NULL
        );

        CREATE TABLE IF NOT EXISTS players (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            number INTEGER NOT NULL,
            salary INTEGER NOT NULL,
            position TEXT NOT NULL,
            team_id INTEGER NOT NULL,
            FOREIGN KEY (team_id) REFERENCES teams(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS player_stats (
            player_id INTEGER PRIMARY KEY,
            attack INTEGER NOT NULL,
            defend INTEGER NOT NULL,
            speed INTEGER NOT NULL,
            strength INTEGER NOT NULL,
            shoot INTEGER NOT NULL,
            pass INTEGER NOT NULL,
            FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE CASCADE
        );
    )sql";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to initialize DB schema: " << errMsg << '\n';
        sqlite3_free(errMsg);
    }
}

void DatabaseManager::initSchemaFromFile(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open schema file: " << filename << '\n';
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql = buffer.str();

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to execute schema: " << errMsg << '\n';
        sqlite3_free(errMsg);
    }
}

bool DatabaseManager::addTeam(const Team& team) 
{
    const char* sql = "INSERT INTO teams (name, salary_cap) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, team.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, team.getSalaryCap());

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::addPlayer(const Player& player, const PlayerStats& stats) {
    // step 1: INSERT player
    const char* sql_player = R"sql(
        INSERT INTO players (name, number, salary, position, team_id)
        VALUES (?, ?, ?, ?, ?);
    )sql";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_player, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, player.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, player.getNumber());
    sqlite3_bind_int64(stmt, 3, player.getSalary());
    sqlite3_bind_text(stmt, 4, player.getPosition().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, player.getTeamId());

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return false;
    }

    int playerId = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);

    // step 2: INSERT player_stats
    const char* sql_stats = R"sql(
        INSERT INTO player_stats (player_id, attack, defend, speed, strength, shoot, pass)
        VALUES (?, ?, ?, ?, ?, ?, ?);
    )sql";

    if (sqlite3_prepare_v2(db, sql_stats, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, playerId);
    sqlite3_bind_int(stmt, 2, stats.attack);
    sqlite3_bind_int(stmt, 3, stats.defend);
    sqlite3_bind_int(stmt, 4, stats.speed);
    sqlite3_bind_int(stmt, 5, stats.strength);
    sqlite3_bind_int(stmt, 6, stats.shoot);
    sqlite3_bind_int(stmt, 7, stats.pass);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

std::vector<Player> DatabaseManager::getPlayersByTeam(int teamId) 
{
    std::vector<Player> result;

    const char* sql = R"sql(
        SELECT id, name, number, salary, position, team_id
        FROM players
        WHERE team_id = ?;
    )sql";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;

    sqlite3_bind_int(stmt, 1, teamId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int number = sqlite3_column_int(stmt, 2);
        long long salary = sqlite3_column_int64(stmt, 3);
        string position = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        int teamId = sqlite3_column_int(stmt, 5);

        Player p(name, number, salary, position, teamId);
        p.setId(id);

        result.push_back(p);
    }

    sqlite3_finalize(stmt);
    return result;
}

bool DatabaseManager::deletePlayer(int playerId) 
{
    const char* sql = "DELETE FROM players WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_int(stmt, 1, playerId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::deleteTeam(int teamId) 
{
    const char* sql = "DELETE FROM teams WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_int(stmt, 1, teamId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::isTeamNameTaken(const std::string& name) 
{
    const char* sql = "SELECT COUNT(*) FROM teams WHERE name = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "Failed to prepare statement for name check: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count > 0;
}

std::optional<int> DatabaseManager::getTeamIdByName(const std::string& name) 
{
    const char* sql = "SELECT id FROM teams WHERE name = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare team ID query: " << sqlite3_errmsg(db) << '\n';
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<int> teamId;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        teamId = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return teamId;
}

std::optional<int> DatabaseManager::getPlayerIdByName(const std::string& name) 
{
    const char* sql = "SELECT id FROM players WHERE name = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare player ID query: " << sqlite3_errmsg(db) << '\n';
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<int> playerId;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        playerId = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return playerId;
}

void DatabaseManager::resetDatabase() 
{
    const char* sql = R"sql(
        DELETE FROM player_stats;
        DELETE FROM players;
        DELETE FROM teams;

        DELETE FROM sqlite_sequence
         WHERE name IN ('player_stats','players','teams');

        VACUUM;
    )sql";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cout << "reset fail: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "reset success\n";
    }
}

void DatabaseManager::executeSqlFile(const string& filename)
 {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "file open fail: " << filename << endl;
        return;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string sql = buffer.str();

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        cout << "execute fail: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "execute success: " << filename << endl;
    }
}

long long DatabaseManager::getTotalSalaryByTeam(int teamId) 
{
    const char* sql = "SELECT SUM(salary) FROM players WHERE team_id = ?;";
    sqlite3_stmt* stmt;
    long long totalSalary = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare salary query\n";
        return -1;
    }

    sqlite3_bind_int(stmt, 1, teamId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        totalSalary = sqlite3_column_int64(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return totalSalary;
}

long long DatabaseManager::getSalaryCapByTeamId(int teamId) 
{
    const char* sql = "SELECT salary_cap FROM teams WHERE id = ?;";
    sqlite3_stmt* stmt;
    long long cap = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare salary cap query\n";
        return -1;
    }

    sqlite3_bind_int(stmt, 1, teamId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        cap = sqlite3_column_int64(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return cap;
}

bool DatabaseManager::printAverageStatsByTeam(int teamId) 
{
    const char* sql = R"sql(
        SELECT 
            AVG(attack), AVG(defend), AVG(speed),
            AVG(strength), AVG(shoot), AVG(pass)
        FROM player_stats
        JOIN players ON player_stats.player_id = players.id
        WHERE players.team_id = ?;
    )sql";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement.\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, teamId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::cout << "Average Stats" << "\n";
        std::cout << "Attack: " << sqlite3_column_double(stmt, 0) << "\n";
        std::cout << "Defend: " << sqlite3_column_double(stmt, 1) << "\n";
        std::cout << "Speed: " << sqlite3_column_double(stmt, 2) << "\n";
        std::cout << "Strength: " << sqlite3_column_double(stmt, 3) << "\n";
        std::cout << "Shoot: " << sqlite3_column_double(stmt, 4) << "\n";
        std::cout << "Pass: " << sqlite3_column_double(stmt, 5) << "\n";
    } else {
        std::cout << "No players found for the team.\n";
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::printPlayerByName(const std::string& name)
{
    const char* sql = R"sql(
        SELECT 
            p.id, p.name, p.number, p.salary, p.position, t.name,
            s.attack, s.defend, s.speed, s.strength, s.shoot, s.pass
        FROM players p
        JOIN teams t ON p.team_id = t.id
        JOIN player_stats s ON p.id = s.player_id
        WHERE p.name = ?;
    )sql";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement.\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        std::cout << "Name: " << sqlite3_column_text(stmt, 1) << '\n';
        std::cout << "Number: " << sqlite3_column_int(stmt, 2) << '\n';
        std::cout << "Salary: " << sqlite3_column_int64(stmt, 3) << '\n';
        std::cout << "Position: " << sqlite3_column_text(stmt, 4) << '\n';
        std::cout << "Team: " << sqlite3_column_text(stmt, 5) << '\n';
        std::cout << "Stats - Attack: " << sqlite3_column_int(stmt, 6)
                  << ", Defend: " << sqlite3_column_int(stmt, 7)
                  << ", Speed: " << sqlite3_column_int(stmt, 8)
                  << ", Strength: " << sqlite3_column_int(stmt, 9)
                  << ", Shoot: " << sqlite3_column_int(stmt, 10)
                  << ", Pass: " << sqlite3_column_int(stmt, 11) << "\n\n";
    }

    if (!found) {
        std::cout << "No player found with that name.\n";
    }

    sqlite3_finalize(stmt);
    return found;
}
