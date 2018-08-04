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
};

struct Team {
  
    std::string m_teamName;
    int m_teamScore;
    int m_wicketDown;
    
    int m_playersPerTeam;
    std::vector<Player*> *m_players;
    
    Team() {
        m_teamName = "";
        m_teamScore = 0;
        m_playersPerTeam = 0;
        m_wicketDown = 0;
        m_players = new std::vector<Player*>();
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
    
};

#endif // __COMMONCLASSES_H__
