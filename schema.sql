-- teams table
CREATE TABLE IF NOT EXISTS teams (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    salary_cap INTEGER NOT NULL CHECK (salary_cap >= 0 AND salary_cap <= 5000000000)
);

-- players table
CREATE TABLE IF NOT EXISTS players (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    number INTEGER NOT NULL,
    salary INTEGER NOT NULL,
    position TEXT NOT NULL,
    team_id INTEGER NOT NULL,
    FOREIGN KEY (team_id) REFERENCES teams(id) ON DELETE CASCADE
);

-- player_stats table
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
