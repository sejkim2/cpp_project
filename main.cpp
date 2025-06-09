#include "DatabaseManager.hpp"
#include <iostream>
#include <limits>
using namespace std;


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

void printMenu()
{
    cout << BOLD << CYAN << "\n--- Football Player Management ---\n" << RESET;
    cout << GREEN << "1. View All Team Names            |\n";
    cout << "2. View Players in a Team         |\n";
    cout << "3. View Team Financial Status     |\n";
    cout << "4. View Average Stats of a Team   |\n" << RESET;
    cout << YELLOW << "5. Search Player by Name          |\n" << RESET;
    cout << BLUE << "6. Add New Team                   |\n";
    cout << "7. Add New Player                 |\n" << RESET;
    cout << RED << "8. Delete Player                  |\n";
    cout << "9. Delete Team                    |\n" << RESET;
    cout << CYAN << "10. Exit                          |\n" << RESET;

    cout << BOLD << "------ Choose one : [1 ~ 10] ------\n> " << RESET;
}

int main() {
    DatabaseManager db("football.db");
    db.initSchemaFromFile("schema.sql");
    db.resetDatabase();
    db.executeSqlFile("dummy.sql");
    // db.initSchema();

    while (true) {
        printMenu();
        
        int choice;
        cin >> choice;
        if (cin.fail()) {
            cout << "Invalid Input\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1)
        {
            cout << "View All Team Names.\n";

            db.printAllTeamNames();
        }
        else if (choice == 2)
        {
            cout << "View Players in a Team\n";
            
            string teamName;

            cout << "Team Name: ";
            cin.ignore();
            getline(cin, teamName);
            
            auto teamIdOpt = db.getTeamIdByName(teamName);
            if (!teamIdOpt.has_value()) 
            {
                std::cout << "Team \"" << teamName << "\" does not exist." << '\n';
                continue;
            }
            auto players = db.getPlayersByTeam(teamIdOpt.value());
            for (auto& p : players) p.print();
        }
        else if (choice == 3)
        {
            cout << "View Team Financial Status.\n";

            string teamName;
            cout << "Team name: ";
            cin.ignore();
            getline(cin, teamName);

            auto teamIdOpt = db.getTeamIdByName(teamName);
            if (!teamIdOpt.has_value()) 
            {
                cout << "Team \"" << teamName << "\" does not exist." << '\n';
                continue;
            }
            else
            {
                long long total = db.getTotalSalaryByTeam(teamIdOpt.value());
                long long cap = db.getSalaryCapByTeamId(teamIdOpt.value());
                cout << "Total salary: " << total << " / Cap: " << cap;
                if (total > cap)
                    cout << " Over Limit!\n";
                else
                    cout << " Within Limit.\n";
            }
        }
        else if (choice == 4)
        {
            cout << "View Average Stats of a Team.\n";

            string teamName;
            cout << "Team Name: ";
            cin.ignore();
            getline(cin, teamName);

            auto teamIdOpt = db.getTeamIdByName(teamName);
            if (!teamIdOpt.has_value())
            {
                std::cout << "Team \"" << teamName << "\" does not exist." << '\n';
                continue;
            }
            db.printAverageStatsByTeam(teamIdOpt.value());
        }
        else if (choice == 5)
        {
            cout << "Search Player by Name.\n";

            string playerName;
            cout << "Enter player name: ";
            cin.ignore();
            getline(std::cin, playerName);

            db.printPlayerByName(playerName);
        }
        else if (choice == 6) {

            cout << "Add new Team\n";
            cout << "**Rule**\n";
            cout << "1. Team name is unique\n";
            cout << "2. Team's salary cap must be between 0 and 500 million inclusive.\n";

            string name;
            long long cap;

            cout << "Team name: ";
            cin.ignore();
            getline(cin, name);

            if (db.isTeamNameTaken(name))
            {
                cout << "This team name already exists. Please choose another." << '\n';
                continue;
            }

            cout << "Salary cap: ";
            cin >> cap;

            try
            {
                Team team(name, cap);
                if (db.addTeam(team))
                    std::cout << "Team added.\n";
                else
                    std::cout << "Failed to add team to database.\n";
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error: " << e.what() << '\n';
            }
        }
        else if (choice == 7)
        {
            cout << "Add new Player\n";
            cout << "**Rule**\n";
            cout << "1. Player's salary must be between 0 and 50 million inclusive.\n";
            cout << "2. Player's stats must be between 0 and 10 inclusive.\n";
            cout << "3. Player's number must be between 1 and 99 inclusive.\n";
            cout << "4. Player's position must be one of the following: CF, LW, SS, AM, RW, LM, CM, RM, DM, LB, CB, RB, or GK.\n";
            cout << "5. Player must belong to a team, and that team must already exist in the system.\n";

            string name, pos, teamName;
            int number;
            long long salary;
            int attack, defend, speed, strength, shoot, pass;

            cout << "Player name: ";
            cin.ignore();
            getline(cin, name);

            cout << "Number: ";
            cin >> number;

            cout << "Salary: ";
            cin >> salary;

            cout << "Position: ";
            cin >> pos;

            cout << "Team Name: ";
            cin.ignore();
            getline(cin, teamName);

            auto teamIdOpt = db.getTeamIdByName(teamName);
            if (!teamIdOpt.has_value()) 
            {
                std::cout << "Team \"" << teamName << "\" does not exist." << '\n';
                continue;
            }

            cout << "Attack: ";
            cin >> attack;
            
            cout << "Defend: ";
            cin >> defend;
            
            cout << "Speed: ";
            cin >> speed;
            
            cout << "Strength: ";
            cin >> strength;
            
            cout << "Shoot: ";
            cin >> shoot;

            cout << "Pass: ";
            cin >> pass;

            try
            {
                int teamId = teamIdOpt.value();

                long long currentTotal = db.getTotalSalaryByTeam(teamId);

                long long cap = db.getSalaryCapByTeamId(teamId);

                if (currentTotal + salary > cap)
                {
                    cout << "Error: Adding this player would exceed the team's salary cap ("
                         << currentTotal << " + " << salary << " > " << cap << ").\n";
                    continue;
                }

                Player player(name, number, salary, pos, teamId);
                PlayerStats stats(0, attack, defend, speed, strength, shoot, pass);

                if (db.addPlayer(player, stats))
                    cout << "Player added.\n";
                else
                    cout << "Failed to add player.\n";
            }
            catch (const std::exception &e)
            {
                cout << "Error: " << e.what() << endl;
            }
        }
        else if (choice == 8) {

            cout << "Delete Player\n";

            string playerName;

            cout << "Enter Player Name to delete: ";
            cin.ignore();
            getline(cin, playerName);

            auto playerIdOpt = db.getPlayerIdByName(playerName);
            if (!playerIdOpt.has_value()) 
            {
                std::cout << "Player \"" << playerName << "\" does not exist." << '\n';
                continue;
            }

            if (db.deletePlayer(playerIdOpt.value()))
                cout << "Player deleted.\n";
            else
                cout << "Failed to delete player.\n";
        }
        else if (choice == 9) {

            cout << "Delete Team\n";

            string teamName;

            cout << "Enter Team Name to delete: ";
            cin.ignore();
            getline(cin, teamName);

            auto teamIdOpt = db.getTeamIdByName(teamName);
            if (!teamIdOpt.has_value()) 
            {
                std::cout << "Team \"" << teamName << "\" does not exist." << '\n';
                continue;;
            }

            if (db.deleteTeam(teamIdOpt.value()))
                cout << "Team deleted (and related players).\n";
            else
                cout << "Failed to delete team.\n";
        }
        else if (choice == 10)
        {
            cout << "Exit.\n";
            cout << "Bye!\n";
            break;
        }
        else {
            cout << "Invalid choice.\n";
            continue;
        }
    }

    return 0;
}
