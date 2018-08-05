//
//  MainScene.hpp
//  AimSSB
//
//  Created by Machintosh on 27/09/16.
//
//

#ifndef MainScene_hpp
#define MainScene_hpp

#include <stdio.h>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "ui/UIWidget.h"
#include "extensions/cocos-ext.h"
#include "ui/UIPageView.h"
#include "commonClasses.h"
#include "ScoreboardLayer.hpp"

using namespace cocos2d::ui;
USING_NS_CC;

class MainScene : public cocos2d::Layer {

    cocos2d::Size visibleSize;
    cocos2d::EventListenerTouchOneByOne *listener;
    cocos2d::EventListenerKeyboard *m_keyboardListener;

public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
   static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

    GameData* m_gameData;
    std::vector<std::string> *m_ballActionVector;
    
    int m_teamTurnIndex;
    int m_ballsTillNow;
    bool m_isAllOverUp;
    std::string m_lastBallAction;
    
    LayerColor* m_backgroundLayer;
    Scale9Sprite* m_playerOne;
    Scale9Sprite* m_playerTwo;
    Sprite* m_teamOneCurrPlayerImage;
    Sprite* m_teamTwoCurrPlayerImage;
    
    int m_currPlayerIndexT1;
    int m_currPlayerIndexT2;
    
    Label* m_currPlayerRunT1Label;
    Label* m_currPlayerBallT1Label;
    Label* m_currPlayerRunT2Label;
    Label* m_currPlayerBallT2Label;
    
    Label* m_currPlayerNameLabelT1;
    Label* m_currPlayerNameLabelT2;
    
    //top & bottom layer elem
    LayerColor* m_bottomLayer;
    LayerColor* m_headerLayer;
    
    Label* m_teamOneNameFooter;
    Label* m_teamTwoNameFooter;
    
    Label* m_teamOneScoreLabel;
    Label* m_teamTwoScoreLabel;
    
    Label* m_headerTeamText;
    Label* m_overLabel;
    Label* m_notificationText;
    Label* m_groundText;
    
    MenuItemSprite* m_ballItem;
    MenuItemSprite* m_nextItem;
    
    Sprite* m_turnArrowOne;
    Sprite* m_turnArrowTwo;
    
    void createPlayerBoxes();
    void createBackground();
    void createScoreBoardIcon();
    
    void dataForBallAction();
    void initialiseGameData();
    
    int getRandomNumber(int min, int max);
    int getRunForBallAction(std::string action);
    
    void createCurrentBatsmanText();
    void createFooterItems();
    void createNotificationBg();
    void createPlayBallButton();
    void ballAnimation(bool isAnimate);
    void createTurnArrow();
    void createGroundImage();
    void createNextButton();
    
    void setAllOverUp(bool overUp);
    bool getIsAllOverUp();
    bool getIsValidDelivery(std::string action);
    bool isWicketDown(std::string action);
    bool isRunExtra(std::string action);
    bool isTeamAllOut(int teamIndex);
    
    void updateGame(float dt);
    
    void hideTurnArrows();
    void setNextGameState(GAME_STATES nextState);
    void setGroundText(std::string message);
    void handleGameOver(RESULT_STATUS matchStatus);
    void handleTeamAllOut(int teamIndex);
    void handleAllOverUp();
    
    void onTurnReceived(int teamIndex);
    
    void updateDataPerBall(int teamIndex, int run, std::string action);
    void updateUIPerBall();
    void updateUIOnWicketFall();
    void updateCurrentPlayerBattingStatus();
    void updateDataOnWicket(int index, std::string action);
    void updateOverLabel();
    void updateTeamScores();
    void updateTeamHeaderText();
    void updateTeamFooterText();
    void updateNotification(std::string notification);
    void updateCurrentPlayerUITeamOne(bool updatePlayerImage = false);
    void updateCurrentPlayerUITeamTwo(bool updatePlayerImage = false);
    
    void playBallItemCallback(Ref* pSender);
    void nextItemCallback(Ref* pSender);
    void scoreBoardCallback(Ref* pSender);
    CREATE_FUNC(MainScene);
    
    virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent);
};

#endif /* MainScene_hpp */
