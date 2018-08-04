#ifndef __COMMONCLASSES_H__
#define __COMMONCLASSES_H__

enum GAME_STATES {
    STATE_YET_TO_START,
    STATE_TOSS,
    STATE_TURN_TEAM_ONE,
    STATE_TURN_TEAM_TWO,
    STATE_OUT_TEAM_ONE,
    STATE_OUT_TEAM_TWO,
    STATE_ANIMATION,
    STATE_RESULT
};

enum BATTING_STATUS {
    BATSMAN_YET_TO_BAT,
    BATSMAN_BATTING,
    BATSMAN_OUT
};

struct Player {
    
    std::string m_playerName;
    std::string m_imagePath;
    BATTING_STATUS m_battingStatus;
    
    int m_runs;
    int m_ballsPlayed;
    bool m_isCaptain;
    bool m_isMyPlayer;
    
    Player () {
        m_playerName = "";
        m_imagePath = "";
        m_battingStatus = BATTING_STATUS::BATSMAN_YET_TO_BAT;
        
        m_runs = 0;
        m_ballsPlayed = 0;
        m_isCaptain = false;
        m_isMyPlayer = false;
    }
    
    Player(const Player* player) {
        m_playerName = player->m_playerName;
        m_imagePath = player->m_imagePath;
        m_runs = player->m_runs;
        m_ballsPlayed = player->m_ballsPlayed;
        m_isCaptain = player->m_isCaptain;
        m_isMyPlayer = player->m_isMyPlayer;
        m_battingStatus = player->m_battingStatus;
    }
    
    Player(const Player& player) {
        m_playerName = player.m_playerName;
        m_imagePath = player.m_imagePath;
        m_runs = player.m_runs;
        m_ballsPlayed = player.m_ballsPlayed;
        m_isCaptain = player.m_isCaptain;
        m_isMyPlayer = player.m_isMyPlayer;
        m_battingStatus = player.m_battingStatus;
    }
};

struct Team {
  
    std::string m_teamName;
    int m_teamScore;
    int m_wicketDown;
    int m_extraRun;
    int m_playersPerTeam;
    std::vector<Player*> *m_players;
    
    Team() {
        m_teamName = "";
        m_teamScore = 0;
        m_playersPerTeam = 0;
        m_wicketDown = 0;
        m_extraRun = 0;
        m_players = new std::vector<Player*>();
    }
    
    Team(const Team* team) {
        m_teamName = team->m_teamName;
        m_teamScore = team->m_teamScore;
        m_wicketDown = team->m_wicketDown;
        m_extraRun = team->m_extraRun;
        m_playersPerTeam = team->m_playersPerTeam;
        
        m_players = new std::vector<Player*>();
        for (int i=0; i < team->m_players->size(); i++) {
            m_players->push_back(new Player(team->m_players->at(i)));
        }
    }
    
    Team(const Team& team) {
        m_teamName = team.m_teamName;
        m_teamScore = team.m_teamScore;
        m_wicketDown = team.m_wicketDown;
        m_extraRun = team.m_extraRun;
        m_playersPerTeam = team.m_playersPerTeam;
        
        m_players = new std::vector<Player*>();
        for (int i=0; i < team.m_players->size(); i++) {
            m_players->push_back(new Player(team.m_players->at(i)));
        }
    }
    
    ~Team() {
        for (int i=0; i < m_players->size(); i++) {
            delete m_players->at(i);
        }
        delete m_players;
        m_players = nullptr;
    }
    
};

struct GameData {
  
    int m_totalOvers;
    int m_currOver;
    int m_numOfBall;
    int m_maxPlayer;
    std::vector<Team*> *m_teamVector;
    GAME_STATES m_gameState;
    
    GameData() {
        m_totalOvers = 0;
        m_currOver = 0;
        m_numOfBall = 0;
        m_maxPlayer = 0;
        m_gameState = GAME_STATES::STATE_YET_TO_START;
        m_teamVector = new std::vector<Team*>();
    }
    
    GameData(const GameData *gameData) {
        m_totalOvers = gameData->m_totalOvers;
        m_currOver = gameData->m_currOver;
        m_numOfBall = gameData->m_numOfBall;
        m_maxPlayer = gameData->m_maxPlayer;
        m_gameState = gameData->m_gameState;
        
        m_teamVector = new std::vector<Team*>();
        for (int i=0; i < gameData->m_teamVector->size(); i++) {
            m_teamVector->push_back(new Team(gameData->m_teamVector->at(i)));
        }
    }
    
    GameData(const GameData &gameData) {
        m_totalOvers = gameData.m_totalOvers;
        m_currOver = gameData.m_currOver;
        m_numOfBall = gameData.m_numOfBall;
        m_maxPlayer = gameData.m_maxPlayer;
        m_gameState = gameData.m_gameState;
        
        m_teamVector = new std::vector<Team*>();
        for (int i=0; i < gameData.m_teamVector->size(); i++) {
            m_teamVector->push_back(new Team(gameData.m_teamVector->at(i)));
        }
    }
    
    ~GameData() {
        for (int i=0; i < m_teamVector->size(); i++) {
            delete m_teamVector->at(i);
        }
        delete m_teamVector;
        m_teamVector = nullptr;
    }
    
};

#endif // __COMMONCLASSES_H__
