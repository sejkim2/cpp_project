# ⚽ Football Player Management System

A console-based football player management program developed using **C++** and **SQLite**.  
Users can register, view, and manage teams and players through a simple number-based menu.

---

## 📌 Features

### 🔹 Player Management
- Register a new player
- Search player by name
- Delete a player

### 🔸 Team Management
- Register a new team (with salary cap range: 0 ~ 5 billion KRW)
- View all existing team names
- View players belonging to a specific team
- Check team’s total salary and financial status
- View average stats of players in a team
- Delete a team (with `CASCADE` → deletes all associated players)

---

## 🗃 Database

- Uses **SQLite** for lightweight, file-based storage
- All data stored in a single `.db` file
- Follows a relational structure with foreign key constraints

### Entity Diagram (ERD)
- `Team`
- `Player`
- `PlayerStats`

Supports `ON DELETE CASCADE` for automatic cleanup

---

## 🖥 How to Run

Clone the repository  
```
bash
git clone https://github.com/yourusername/football-manager.git
make (Make sure you run this command in the same directory where the Makefile is located.)
```
  
