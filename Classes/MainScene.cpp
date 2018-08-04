//
//  MainScene.cpp
//  AimSSB
//
//  Created by Machintosh on 27/09/16.
//
//

#include "MainScene.hpp"
#include "AppDelegate.h"
#include "constant.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

USING_NS_CC_EXT;
using namespace std;

#define FONT_SIZE_MENUITEM                  27
#define MAX_PLAYER                          5
#define MAX_OVER                            5

#define BALL_ANIMATION_TAG                  1001
#define TIME_DURATION                       1

Scene* MainScene::createScene(){
    auto scene = Scene::create();
    // 'layer' is an autorelease object
    auto layer = MainScene::create();
    // add layer as a child to scene
    scene->addChild(layer);
    // return the scene
    return scene;
}

bool MainScene::init(){
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() ){
        return false;
    }
    
    m_gameData = nullptr;
    m_headerTeamText = nullptr;
    m_keyboardListener = NULL;
    m_ballActionVector = nullptr;
    m_teamTurnIndex = 0;
    m_ballCurrOver = 0;
    
    return true;
}

void MainScene::onEnter(){
    Layer::onEnter();
    Node::onEnter();
    
    visibleSize = Director::getInstance()->getVisibleSize();

    initialiseGameData();
    dataForBallAction();
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [=](Touch* touch, Event* event){
        return true;
    };
    listener->setSwallowTouches(true);
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    listener->retain();

    m_keyboardListener = EventListenerKeyboard::create();
    m_keyboardListener->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_keyboardListener, this);
    
    createBackground();
    createPlayerBoxes();
    createCurrentBatsmanText();
    createFooterItems();
    createNotificationBg();
    
    updateTeamHeaderText();
    updateTeamFooterText();
    updateOverLabel();
    updateTeamScores();
    createPlayBallButton();
    
    createTurnArrow();
    createGroundImage();
    createNextButton();
    
    this->schedule(CC_SCHEDULE_SELECTOR(MainScene::updateGame), TIME_DURATION);
}

void MainScene::updateGame(float dt) {
    CCLOG("inside update game");
    if (m_gameData != nullptr) {
        
        GAME_STATES currState = m_gameData->m_gameState;
        switch (currState) {
            
            case STATE_TOSS: {
                CCLOG("update game toss");
            }
            break;
                
            case STATE_YET_TO_START: {
                CCLOG("update game yet to start");
                updateNotification("WELCOME");
                setNextGameState(STATE_TOSS);
            }
            break;
                
            case STATE_TURN_TEAM_TWO: {
                CCLOG("update game turn team two");
                onTurnReceived(m_teamTurnIndex);
            }
                break;
                
            case STATE_TURN_TEAM_ONE: {
                CCLOG("update game turn team one");
                onTurnReceived(m_teamTurnIndex);
            }
                break;
                
            case STATE_ANIMATION: {
                
                bool isDeliveryValid = getIsValidDelivery(m_lastBallAction);
                if (isDeliveryValid) {
                    if (m_teamTurnIndex == 0) {
                        m_teamTurnIndex = 1;
                    }else {
                        m_teamTurnIndex = 0;
                    }
                }
                if (m_teamTurnIndex == 0)
                    setNextGameState(STATE_TURN_TEAM_ONE);
                else
                    setNextGameState(STATE_TURN_TEAM_TWO);
                
                //setNextGameState(STATE_TURN_TEAM_TWO);
            }
                break;
            case STATE_OUT_TEAM_ONE: {
                CCLOG("update game out team one");
                
            }
                break;
                
            case STATE_OUT_TEAM_TWO: {
                CCLOG("update game out team two");
            }
                break;
                
            case STATE_RESULT: {
                
                CCLOG("update game result");
            }
                break;
                
            default:
                break;
        }
        
    }
}

void MainScene::setGroundText(std::string message) {
    m_groundText->setString(message);
}

void MainScene::setNextGameState(GAME_STATES nextState) {
    if (m_gameData != nullptr) {
        m_gameData->m_gameState = nextState;
    }
}

void MainScene::createGroundImage() {
    
    Scale9Sprite* groundImage = Scale9Sprite::create(IMAGE_PATH"board_9patch.png");
    groundImage->setContentSize(Size(visibleSize.width * 0.32, visibleSize.height* 0.3));
    groundImage->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.675));
    this->addChild(groundImage);
    
    m_groundText = Label::createWithTTF("Hi ! \n I am Ravi Shastri. \n Your Commentator of the Match",FONT_DOSIS, 45);
    m_groundText->setAnchorPoint(Vec2(0.5, 0.5));
    m_groundText->setHorizontalAlignment(TextHAlignment::CENTER);
    m_groundText->setWidth(groundImage->getContentSize().width * 0.95);
    m_groundText->setColor(Color3B::BLACK);
    m_groundText->setPosition(Vec2(groundImage->getContentSize().width * 0.5,
                                          groundImage->getContentSize().height * 0.5));
    groundImage->addChild(m_groundText);
}

void MainScene::hideTurnArrows() {
    m_turnArrowOne->setVisible(false);
    m_turnArrowTwo->setVisible(false);
}

void MainScene::onTurnReceived(int teamIndex) {
    
    std::string teamName = m_gameData->m_teamVector->at(teamIndex)->m_teamName;
    std::string message = teamName + "'s TURN";
    setGroundText(message);
    updateNotification("Waiting...");
    
    if (teamIndex == 0 && !m_turnArrowOne->isVisible()) {
        m_turnArrowOne->setVisible(true);
        m_turnArrowTwo->stopAllActions();
        m_turnArrowTwo->setVisible(false);
        
        MoveBy* moveForward = MoveBy::create(0.5, Vec2(10,0));
        MoveBy* moveBackward = MoveBy::create(0.5, Vec2(-10,0));
        Sequence* animationAction = Sequence::create(moveForward, moveBackward,NULL);
        m_turnArrowOne->runAction(RepeatForever::create(animationAction));
        ballAnimation(true);
        
    }else if (teamIndex == 1 && !m_turnArrowTwo->isVisible()) {
        
        m_turnArrowOne->setVisible(false);
        m_turnArrowOne->stopAllActions();
        m_turnArrowTwo->setVisible(true);
        
        MoveBy* moveForward = MoveBy::create(0.5, Vec2(-10,0));
        MoveBy* moveBackward = MoveBy::create(0.5, Vec2(10,0));
        Sequence* animationAction = Sequence::create(moveForward, moveBackward,NULL);
        m_turnArrowTwo->runAction(RepeatForever::create(animationAction));
        ballAnimation(true);
    }
}

void MainScene::createTurnArrow() {
    
    m_turnArrowOne = Sprite::create(IMAGE_PATH"turnArrow.png");
    m_turnArrowOne->setScale(0.6);
    m_turnArrowOne->setPosition(Vec2(visibleSize.width * 0.375, visibleSize.height * 0.37));
    m_turnArrowOne->setVisible(false);
    this->addChild(m_turnArrowOne);
    
    m_turnArrowTwo = Sprite::create(IMAGE_PATH"turnArrow.png");
    m_turnArrowTwo->setScale(0.6);
    m_turnArrowTwo->setRotation(180);
    m_turnArrowTwo->setVisible(false);
    m_turnArrowTwo->setPosition(Vec2(visibleSize.width * 0.625, visibleSize.height * 0.37));
    this->addChild(m_turnArrowTwo);
    
}

void MainScene::updateUIPerBall() {
    
    updateOverLabel();
    updateTeamScores();
    
    updateCurrentPlayerUITeamOne();
    updateCurrentPlayerUITeamTwo();
}

void MainScene::handleGameOver() {
    CCLOG("handle game over");
}

void MainScene::updateDataOnWicket(int index, std::string action) {
    
    if (m_gameData != nullptr){
        int playerIndex = 0;
        if (index == 0)
            playerIndex = m_currPlayerIndexT1;
        else
            playerIndex = m_currPlayerIndexT2;
            
        m_gameData->m_teamVector->at(index)->m_wicketDown += 1;
        m_gameData->m_teamVector->at(index)->m_players->at(playerIndex)->m_battingStatus = BATTING_STATUS::BATSMAN_OUT;
        
        if (index == 0) {
            m_currPlayerIndexT1++;
            if (m_currPlayerIndexT1 >= m_gameData->m_maxPlayer) {
                m_currPlayerIndexT1 = m_gameData->m_maxPlayer - 1;
                handleGameOver();
            }
        }else{
            m_currPlayerIndexT2++;
            if (m_currPlayerIndexT2 >= m_gameData->m_maxPlayer) {
                m_currPlayerIndexT2 = m_gameData->m_maxPlayer - 1;
                handleGameOver();
            }
        }
    }
    
}

void MainScene::updateUIOnWicketFall() {
    updateCurrentPlayerUITeamOne(true);
    updateCurrentPlayerUITeamTwo(true);
    updateTeamScores();
    
}

void MainScene::updateCurrentPlayerBattingStatus() {
    
    if (m_gameData != nullptr) {
        m_gameData->m_teamVector->at(0)->m_players->at(m_currPlayerIndexT1)->m_battingStatus = BATTING_STATUS::BATSMAN_BATTING;
        m_gameData->m_teamVector->at(1)->m_players->at(m_currPlayerIndexT2)->m_battingStatus = BATTING_STATUS::BATSMAN_BATTING;
    }
}

void MainScene::nextItemCallback(Ref* pSender) {
    CCLOG("Inside next item");
    static int messageCount = 0;
    std::string message = "";
    if (messageCount == 0) {
        message = "Two Captains out in the middle for toss. \n Let's Toss";
        setGroundText(message);
        messageCount++;
        return;
    }else if (messageCount == 1) {
        std::string teamName = m_gameData->m_teamVector->at(0)->m_teamName;
        message = teamName + " won the Toss and will face the first ball";
        setGroundText(message);
        updateNotification("TOSS");
        messageCount++;
        return;
    }else if (messageCount == 2) {
        message = "Both the Openers are out in the middle now.";
        setGroundText(message);
        messageCount++;
        m_currPlayerIndexT1 = 0;
        m_currPlayerIndexT2 = 0;
        updateCurrentPlayerBattingStatus();
        updateCurrentPlayerUITeamOne(true);
        updateCurrentPlayerUITeamTwo(true);
        return;
    }else if (messageCount == 3) {
        message = "Both team will play simultaeneously. Same ball will be faced by both the teams";
        setGroundText(message);
        messageCount++;
        return;
    }else if (messageCount == 4) {
        message = "Team scoring maximum runs after 5 overs will win. \n Good Luck !";
        setGroundText(message);
        messageCount++;
        return;
    }else if (messageCount == 5) {
        m_nextItem->setVisible(false);
        m_nextItem->setEnabled(false);
        setGroundText("");
        m_teamTurnIndex = 0;
        updateNotification("PLAY");
        setNextGameState(STATE_TURN_TEAM_ONE);
        return;
    }
}

int MainScene::getRandomNumber(int min, int max) {
    srand(time(0));
    int num = (rand() % (max - min + 1)) + min;
    CCLOG("Index is %d", num);
    return num;
}

void MainScene::updateDataPerBall(int teamIndex, int run, std::string action) {
    
    if (m_gameData != nullptr) {
        int playerIndex = 0;
        
        if (teamIndex == 0)
            playerIndex = m_currPlayerIndexT1;
        else
            playerIndex = m_currPlayerIndexT2;
        
        bool isDeliveryValid = getIsValidDelivery(action);
        m_gameData->m_teamVector->at(teamIndex)->m_teamScore += run;
        m_gameData->m_teamVector->at(teamIndex)->m_players->at(playerIndex)->m_runs += run;
            
        if (isDeliveryValid) {
            m_gameData->m_teamVector->at(teamIndex)->m_players->at(playerIndex)->m_ballsPlayed += 1;
            
            if (m_ballCurrOver % 2 == 0) {
                int ballInCurrOver = (int)(m_ballCurrOver/2) % 6;
                m_gameData->m_numOfBall = ballInCurrOver;
                m_gameData->m_currOver = (int)(m_ballCurrOver/2)/6;
            }
        }
    }
}

void MainScene::playBallItemCallback(Ref* pSender){
    CCLOG("inside play callback");
    if ( (m_gameData->m_gameState == GAME_STATES::STATE_TURN_TEAM_ONE) || (m_gameData->m_gameState == GAME_STATES::STATE_TURN_TEAM_TWO)) {
        
        ballAnimation(false);
        hideTurnArrows();
        
        int index = getRandomNumber(0, (int)m_ballActionVector->size() -1);
        m_lastBallAction = m_ballActionVector->at(index);
        int runs = getRunForBallAction(m_lastBallAction);
        
        if (getIsValidDelivery(m_lastBallAction))
            m_ballCurrOver++;

        updateNotification(m_lastBallAction);
        updateDataPerBall(m_teamTurnIndex, runs, m_lastBallAction);
        updateUIPerBall();
        
        if (isWicketDown(m_lastBallAction)) {
            updateDataOnWicket(m_teamTurnIndex, m_lastBallAction);
            updateUIOnWicketFall();
        }
            
        setNextGameState(STATE_ANIMATION);
    }
}

void MainScene::dataForBallAction(){
    
    if (m_ballActionVector == nullptr)
        m_ballActionVector = new std::vector<std::string>();
    
    m_ballActionVector->push_back("Single");
    m_ballActionVector->push_back("Dot Ball");
    m_ballActionVector->push_back("Double");
    m_ballActionVector->push_back("Triple");
    m_ballActionVector->push_back("Run Out");
    m_ballActionVector->push_back("Four");
    m_ballActionVector->push_back("No Ball");
    m_ballActionVector->push_back("Six");
    m_ballActionVector->push_back("1 By Run");
    m_ballActionVector->push_back("Wide Ball");
    m_ballActionVector->push_back("Out");
    m_ballActionVector->push_back("1 Leg By Run");
}

bool MainScene::isWicketDown(std::string action) {
    
    bool result = false;
    if (action == "Out" || action == "Run Out")
        result = true;
    
    return result;
    
}

bool MainScene::getIsValidDelivery(std::string action) {
    bool result = true;
    
    if (action == "No Ball" || action == "Wide Ball")
        result = false;
    
    return result;
}

int MainScene::getRunForBallAction(std::string action) {
    
    int  result = 0;
    if (action == "Single") {
        result = 1;
    }else if (action == "Double") {
        result = 2;
    }else if (action == "Triple") {
        result = 3;
    }else if (action == "Four") {
        result = 4;
    }else if (action == "Six") {
        result = 6;
    }else if (action == "No Ball" || action == "Wide Ball" || action == "1 Leg By Run" || action == "1 By Run") {
        result = 1;
    }
    
    return result;
}

void MainScene::updateOverLabel() {
    
    if (m_gameData != nullptr && m_overLabel != nullptr) {
        int totalOvers = m_gameData->m_totalOvers;
        int currOver = m_gameData->m_currOver;
        int balls = m_gameData->m_numOfBall;
        
        std::string overText = std::to_string(currOver) + "." + std::to_string(balls) + "/" + std::to_string(totalOvers);
        m_overLabel->setString(overText);
    }
}

void MainScene::updateTeamScores() {
    
    if (m_gameData != nullptr){
        
        int totalRunsOne = m_gameData->m_teamVector->at(0)->m_teamScore;
        int wicketDownOne = m_gameData->m_teamVector->at(0)->m_wicketDown;
        
        std::string scoreTextOne = std::to_string(totalRunsOne) + "/" + std::to_string(wicketDownOne);
        m_teamOneScoreLabel->setString(scoreTextOne);
        
        int totalRunsTwo = m_gameData->m_teamVector->at(1)->m_teamScore;
        int wicketDownTwo = m_gameData->m_teamVector->at(1)->m_wicketDown;
        
        std::string scoreTextTwo = std::to_string(totalRunsTwo) + "/" + std::to_string(wicketDownTwo);
        m_teamTwoScoreLabel->setString(scoreTextTwo);
        
    }
}

void MainScene::updateTeamFooterText() {
    
    if (m_gameData != nullptr) {
        m_teamOneNameFooter->setString(m_gameData->m_teamVector->at(0)->m_teamName);
        m_teamTwoNameFooter->setString(m_gameData->m_teamVector->at(1)->m_teamName);
        
        m_teamOneScoreLabel->setPosition(Vec2(m_teamOneNameFooter->getPositionX() + m_teamOneNameFooter->getContentSize().width * 1.1,
                                              m_bottomLayer->getContentSize().height * 0.5));
        
        m_teamTwoScoreLabel->setPosition(Vec2(m_teamTwoNameFooter->getPositionX() - m_teamTwoNameFooter->getContentSize().width * 1.1,
                                              m_bottomLayer->getContentSize().height * 0.5));
    }
}

void MainScene::ballAnimation(bool isAnimate) {
    
    if (m_ballItem != nullptr) {
        
        if (isAnimate) {
            
            MoveBy* moveActionUp = MoveBy::create(1, Vec2(0,15));
            MoveBy* moveActionDown = MoveBy::create(1, Vec2(0,-15));
            MoveBy* moveActionUp2 = MoveBy::create(0.5, Vec2(0,8));
            MoveBy* moveActionDown2 = MoveBy::create(0.5, Vec2(0,-8));
            Sequence* animationAction = Sequence::create(moveActionUp, moveActionDown,moveActionUp2,moveActionDown2,NULL);
            
            animationAction->setTag(BALL_ANIMATION_TAG);
            m_ballItem->runAction(RepeatForever::create(animationAction));
        }else {
            m_ballItem->stopAllActions();
            m_ballItem->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.37));
        }
    }
}

void MainScene::createNextButton() {
    
    Scale9Sprite* nextSprite =  Scale9Sprite::create("scaleableImage.png");
    nextSprite->setContentSize(Size(130, 65));
    
    Label* nextLabel = Label::createWithTTF("NEXT",FONT_DOSIS, 40);
    nextLabel->setAnchorPoint(Vec2(0.5, 0.5));
    nextLabel->setPosition(Vec2(nextSprite->getContentSize().width * 0.5 ,
                                nextSprite->getContentSize().height * 0.5));
    nextSprite->addChild(nextLabel);
    
    m_nextItem = MenuItemSprite::create(nextSprite, nextSprite,CC_CALLBACK_1(MainScene::nextItemCallback, this));
    m_nextItem->setPosition(Vec2(visibleSize.width * 0.625, visibleSize.height * 0.475));
    
    auto ballMenu = Menu::create(m_nextItem, NULL);
    ballMenu->setPosition(Vec2(0,0));
    this->addChild(ballMenu);
}

void MainScene::createPlayBallButton() {
    
    Sprite* ballSprite =  Sprite::create(IMAGE_PATH"ball.png");
    
    m_ballItem = MenuItemSprite::create(ballSprite, ballSprite,CC_CALLBACK_1(MainScene::playBallItemCallback, this));
    m_ballItem->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.37));
    
    auto ballMenu = Menu::create(m_ballItem, NULL);
    ballMenu->setPosition(Vec2(0,0));
    this->addChild(ballMenu);
}

void MainScene::createFooterItems() {
    
    Label* overTextLabel = Label::createWithTTF("OVERS:",FONT_HEADLINE, 50);
    overTextLabel->setAnchorPoint(Vec2(1, 0.5));
    overTextLabel->setPosition(Vec2(m_bottomLayer->getContentSize().width * 0.5 - overTextLabel->getContentSize().width * 0.05,
                                     m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(overTextLabel);
    
    
    m_overLabel = Label::createWithTTF("",FONT_HEADLINE,50);
    m_overLabel->setAnchorPoint(Vec2(0, 0.5));
    m_overLabel->setPosition(Vec2(m_bottomLayer->getContentSize().width * 0.5,
                                     m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_overLabel);
    
    m_teamOneNameFooter = Label::createWithTTF("",FONT_HEADLINE, 40);
    m_teamOneNameFooter->setAnchorPoint(Vec2(0, 0.5));
    m_teamOneNameFooter->setPosition(Vec2(20,
                                    m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_teamOneNameFooter);
    
    m_teamTwoNameFooter = Label::createWithTTF("",FONT_HEADLINE, 40);
    m_teamTwoNameFooter->setAnchorPoint(Vec2(1, 0.5));
    m_teamTwoNameFooter->setPosition(Vec2(m_bottomLayer->getContentSize().width - 20,
                                  m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_teamTwoNameFooter);
    
    
    m_teamOneScoreLabel = Label::createWithTTF("",FONT_HEADLINE, 50);
    m_teamOneScoreLabel->setAnchorPoint(Vec2(0, 0.5));
    m_teamOneScoreLabel->setPosition(Vec2(m_teamOneNameFooter->getPositionX() + m_teamOneNameFooter->getContentSize().width * 1.1,
                                          m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_teamOneScoreLabel);

    m_teamTwoScoreLabel = Label::createWithTTF("",FONT_HEADLINE, 50);
    m_teamTwoScoreLabel->setAnchorPoint(Vec2(1, 0.5));
    m_teamTwoScoreLabel->setPosition(Vec2(m_teamTwoNameFooter->getPositionX() - m_teamTwoNameFooter->getContentSize().width * 1.1,
                                          m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_teamTwoScoreLabel);
    
}

void MainScene::updateCurrentPlayerUITeamOne(bool updatePlayerImage) {
    
    if (m_gameData != nullptr) {
        Player* currPlayerTeam1 =  m_gameData->m_teamVector->at(0)->m_players->at(m_currPlayerIndexT1);
        m_currPlayerRunT1Label->setString(std::to_string(currPlayerTeam1->m_runs));
        m_currPlayerBallT1Label->setString(std::to_string(currPlayerTeam1->m_ballsPlayed));
        
        if (updatePlayerImage) {
            Sprite* temp = Sprite::create(PLAYER_IMAGE_PATH + currPlayerTeam1->m_imagePath);
            m_teamOneCurrPlayerImage->setTexture(temp->getTexture());
            m_currPlayerNameLabelT1->setString(currPlayerTeam1->m_playerName);
        }
    }
}

void MainScene::updateCurrentPlayerUITeamTwo(bool updatePlayerImage) {
    
    if (m_gameData != nullptr) {
        Player* currPlayerTeam2 =  m_gameData->m_teamVector->at(1)->m_players->at(m_currPlayerIndexT2);
        m_currPlayerRunT2Label->setString(std::to_string(currPlayerTeam2->m_runs));
        m_currPlayerBallT2Label->setString(std::to_string(currPlayerTeam2->m_ballsPlayed));
        
        if (updatePlayerImage) {
            Sprite* temp = Sprite::create(PLAYER_IMAGE_PATH + currPlayerTeam2->m_imagePath);
            m_teamTwoCurrPlayerImage->setTexture(temp->getTexture());
            m_currPlayerNameLabelT2->setString(currPlayerTeam2->m_playerName);
        }
    }
    
}

void MainScene::createPlayerBoxes() {
    
    //bg
    m_playerOne = Scale9Sprite::create(IMAGE_PATH"playerBg.png");
    m_playerOne->setContentSize(Size(visibleSize.width * 0.3, visibleSize.height * 0.7));
    m_playerOne->setAnchorPoint(Vec2(0, 0.5));
    m_playerOne->setPosition(Vec2(m_playerOne->getContentSize().width * 0.05, visibleSize.height * 0.525));
    this->addChild(m_playerOne);
    
    //image
    m_teamOneCurrPlayerImage = Sprite::create(PLAYER_IMAGE_PATH"default.jpg");
    Size ws = Size(m_playerOne->getContentSize().width * 0.85, m_playerOne->getContentSize().height * 0.65);
    Size maskSize = ws* 0.975;
    float radius = 30;
    int scaleBoundsX = m_playerOne->getContentSize().width * 0.85;
    int scaleBoundY = m_playerOne->getContentSize().height * 0.65;
    float scaleX = scaleBoundsX/m_teamOneCurrPlayerImage->getContentSize().width;
    float scaleY = scaleBoundY/m_teamOneCurrPlayerImage->getContentSize().height;
    m_teamOneCurrPlayerImage->setScaleX(scaleX);
    m_teamOneCurrPlayerImage->setScaleY(scaleY);
    
    // create masked image and position to center it on screen
    Node* clipNode = AppDelegate::getDelegate()->createRoundedRectMaskNode(maskSize, radius, 1.0f, 10);
    m_teamOneCurrPlayerImage->setAnchorPoint(Vec2(0,0));
    clipNode->setAnchorPoint(Vec2(0.5,0.5));
    clipNode->addChild(m_teamOneCurrPlayerImage);
    clipNode->setPosition(Vec2(m_playerOne->getContentSize().width * 0.075,
                               m_playerOne->getContentSize().height * 0.325));
    m_playerOne->addChild(clipNode);
    
    //default texts
    Label* batsmanRunText = Label::createWithTTF("RUNS :",FONT_DOSIS,40);
    batsmanRunText->setAnchorPoint(Vec2(0, 0.5));
    batsmanRunText->setPosition(Vec2(m_playerOne->getContentSize().width * 0.1,
                                     m_playerOne->getContentSize().height * 0.175));
    m_playerOne->addChild(batsmanRunText);
    
    Label* batsmanBallFacedText = Label::createWithTTF("BALLS :",FONT_DOSIS,40);
    batsmanBallFacedText->setAnchorPoint(Vec2(0, 0.5));
    batsmanBallFacedText->setPosition(Vec2(m_playerOne->getContentSize().width * 0.1,
                                           m_playerOne->getContentSize().height * 0.1));
    m_playerOne->addChild(batsmanBallFacedText);
    
    
    m_currPlayerRunT1Label = Label::createWithTTF("",FONT_DOSIS,40);
    m_currPlayerRunT1Label->setAnchorPoint(Vec2(1, 0.5));
    m_currPlayerRunT1Label->setPosition(Vec2(m_playerOne->getContentSize().width - 40,
                                     m_playerOne->getContentSize().height * 0.175));
    m_playerOne->addChild(m_currPlayerRunT1Label);
    
    m_currPlayerBallT1Label = Label::createWithTTF("",FONT_DOSIS,40);
    m_currPlayerBallT1Label->setAnchorPoint(Vec2(1, 0.5));
    m_currPlayerBallT1Label->setPosition(Vec2(m_playerOne->getContentSize().width - 40,
                                           m_playerOne->getContentSize().height * 0.1));
    m_playerOne->addChild(m_currPlayerBallT1Label);

    m_currPlayerNameLabelT1 = Label::createWithTTF("BATSMAN",FONT_DOSIS,50);
    m_currPlayerNameLabelT1->setAnchorPoint(Vec2(0.5, 0.5));
    m_currPlayerNameLabelT1->setPosition(Vec2(m_playerOne->getContentSize().width * 0.5,
                                              m_playerOne->getContentSize().height * 0.275));
    m_playerOne->addChild(m_currPlayerNameLabelT1);
    
    //Player 2 bg
    m_playerTwo = Scale9Sprite::create(IMAGE_PATH"playerBg.png");
    m_playerTwo->setContentSize(Size(visibleSize.width * 0.3, visibleSize.height * 0.7));
    m_playerTwo->setAnchorPoint(Vec2(1, 0.5));
    m_playerTwo->setPosition(Vec2(visibleSize.width - m_playerTwo->getContentSize().width * 0.05, visibleSize.height * 0.525));
    this->addChild(m_playerTwo);
    
    m_teamTwoCurrPlayerImage = Sprite::create(PLAYER_IMAGE_PATH"default2.jpg");
    ws = Size(m_playerTwo->getContentSize().width * 0.85, m_playerTwo->getContentSize().height * 0.65);
    maskSize = ws* 0.975;
    radius = 30;
    
    
    scaleBoundsX = m_playerTwo->getContentSize().width * 0.85;
    scaleBoundY = m_playerTwo->getContentSize().height * 0.65;
    scaleX = scaleBoundsX/m_teamTwoCurrPlayerImage->getContentSize().width;
    scaleY = scaleBoundY/m_teamTwoCurrPlayerImage->getContentSize().height;
    m_teamTwoCurrPlayerImage->setScaleX(scaleX);
    m_teamTwoCurrPlayerImage->setScaleY(scaleY);
    
    // create masked image and position to center it on screen
    Node* clipNode2 = AppDelegate::getDelegate()->createRoundedRectMaskNode(maskSize, radius, 1.0f, 10);
    m_teamTwoCurrPlayerImage->setAnchorPoint(Vec2(0,0));
    clipNode2->setAnchorPoint(Vec2(0.5,0.5));
    clipNode2->addChild(m_teamTwoCurrPlayerImage);
    clipNode2->setPosition(Vec2(m_playerTwo->getContentSize().width * 0.075, m_playerTwo->getContentSize().height * 0.325));
    m_playerTwo->addChild(clipNode2);
    
    //default texts
    Label* batsmanRunText2 = Label::createWithTTF("RUNS :",FONT_DOSIS,40);
    batsmanRunText2->setAnchorPoint(Vec2(0, 0.5));
    batsmanRunText2->setPosition(Vec2(m_playerTwo->getContentSize().width * 0.1,
                                     m_playerTwo->getContentSize().height * 0.175));
    m_playerTwo->addChild(batsmanRunText2);
    
    Label* batsmanBallFacedText2 = Label::createWithTTF("BALLS :",FONT_DOSIS,40);
    batsmanBallFacedText2->setAnchorPoint(Vec2(0, 0.5));
    batsmanBallFacedText2->setPosition(Vec2(m_playerTwo->getContentSize().width * 0.1,
                                           m_playerTwo->getContentSize().height * 0.1));
    m_playerTwo->addChild(batsmanBallFacedText2);
    
    
    m_currPlayerRunT2Label = Label::createWithTTF("",FONT_DOSIS,40);
    m_currPlayerRunT2Label->setAnchorPoint(Vec2(1, 0.5));
    m_currPlayerRunT2Label->setPosition(Vec2(m_playerTwo->getContentSize().width - 40,
                                             m_playerTwo->getContentSize().height * 0.175));
    m_playerTwo->addChild(m_currPlayerRunT2Label);
    
    m_currPlayerBallT2Label = Label::createWithTTF("",FONT_DOSIS,40);
    m_currPlayerBallT2Label->setAnchorPoint(Vec2(1, 0.5));
    m_currPlayerBallT2Label->setPosition(Vec2(m_playerTwo->getContentSize().width - 40,
                                              m_playerTwo->getContentSize().height * 0.1));
    m_playerTwo->addChild(m_currPlayerBallT2Label);
    
    m_currPlayerNameLabelT2 = Label::createWithTTF("BATSMAN",FONT_DOSIS,50);
    m_currPlayerNameLabelT2->setAnchorPoint(Vec2(0.5, 0.5));
    m_currPlayerNameLabelT2->setPosition(Vec2(m_playerTwo->getContentSize().width * 0.5,
                                              m_playerTwo->getContentSize().height * 0.275));
    m_playerTwo->addChild(m_currPlayerNameLabelT2);
    
}

void MainScene::updateTeamHeaderText() {
    
    if (m_headerTeamText != nullptr && m_gameData != nullptr) {
        std::string teamText = m_gameData->m_teamVector->at(0)->m_teamName + " " + "V" + " "
        + m_gameData->m_teamVector->at(1)->m_teamName;
        
        m_headerTeamText->setString(teamText);
    }
}

void MainScene::createBackground(){

    m_backgroundLayer = LayerColor::create(Color4B::WHITE, visibleSize.width, visibleSize.height);
    m_backgroundLayer->setPosition(Vec2(0,0));
    this->addChild(m_backgroundLayer);
    
    m_headerLayer = LayerColor::create(Color4B(26,82,118,255), visibleSize.width, visibleSize.height * 0.125);
    m_headerLayer->setPosition(Vec2(0, visibleSize.height - visibleSize.height * 0.125));
    this->addChild(m_headerLayer);
    
    m_headerTeamText= Label::createWithTTF("",FONT_HEADLINE,60);
    m_headerTeamText->setPosition(Vec2(m_headerLayer->getContentSize().width * 0.5, m_headerLayer->getContentSize().height * 0.5));
    m_headerLayer->addChild(m_headerTeamText);

    m_bottomLayer = LayerColor::create(Color4B(26,82,118,255), visibleSize.width, visibleSize.height * 0.125);
    m_bottomLayer->setPosition(Vec2(0, 0));
    this->addChild(m_bottomLayer);
}

void MainScene::createCurrentBatsmanText() {
    
    if (m_gameData != nullptr) {
        std::string textStr = m_gameData->m_teamVector->at(0)->m_teamName + " : CURRENT BATSMAN";
        Label *teamOneCurrBatsmanText= Label::createWithTTF(textStr,FONT_DOSIS,35);
        teamOneCurrBatsmanText->setColor(Color3B::BLACK);
        
        teamOneCurrBatsmanText->setAnchorPoint(Vec2(0, 0.5));
        teamOneCurrBatsmanText->setPosition(Vec2(m_playerOne->getContentSize().width * 0.08,
                                                 visibleSize.height * 0.525 - m_playerOne->getContentSize().height * 0.5175));
        
        this->addChild(teamOneCurrBatsmanText);
        
        std::string textStr2 = m_gameData->m_teamVector->at(1)->m_teamName + " : CURRENT BATSMAN";
        Label *teamTwoCurrBatsmanText= Label::createWithTTF(textStr2,FONT_DOSIS,35);
        teamTwoCurrBatsmanText->setColor(Color3B::BLACK);
        
        teamTwoCurrBatsmanText->setAnchorPoint(Vec2(1, 0.5));
        teamTwoCurrBatsmanText->setPosition(Vec2(visibleSize.width - m_playerTwo->getContentSize().width * 0.08,
                                                 visibleSize.height * 0.525 - m_playerOne->getContentSize().height * 0.5175));
        
        this->addChild(teamTwoCurrBatsmanText);
    }
}

void MainScene::updateNotification(std::string notification) {
    
    if (m_notificationText != nullptr) {
        m_notificationText->setString(notification);
    }
}

void MainScene::createNotificationBg() {
 
    Scale9Sprite* notificationBg = Scale9Sprite::create(IMAGE_PATH"notificationBg.png");
    notificationBg->setContentSize(Size(visibleSize.width * 0.25, visibleSize.height * 0.1));
    notificationBg->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.225));
    this->addChild(notificationBg);
    
    m_notificationText = Label::createWithTTF("WELCOME",FONT_ARM_WRESTLER,60);
    m_notificationText->setAnchorPoint(Vec2(0.5, 0.5));
    m_notificationText->setPosition(Vec2(notificationBg->getContentSize().width * 0.5,
                                             notificationBg->getContentSize().height * 0.5));
    notificationBg->addChild(m_notificationText);
}

void MainScene::initialiseGameData() {
    
    if (m_gameData != nullptr)
        delete m_gameData;
    
    m_gameData = new GameData();
    
    
    m_gameData->m_maxPlayer = MAX_PLAYER;
    m_gameData->m_totalOvers = MAX_OVER;
    m_gameData->m_currOver = 0;
    m_gameData->m_numOfBall = 0;
    
    Team* teamOne = new Team();
    teamOne->m_teamName = "INDIA";
    teamOne->m_playersPerTeam = MAX_PLAYER;
    
    Player* playerOne = new Player();
    playerOne->m_isMyPlayer = false;
    playerOne->m_isCaptain = true;
    playerOne->m_playerName = "Virat Kohli";
    playerOne->m_imagePath = "viratKohli.jpg";
    
    Player* playerTwo = new Player();
    playerTwo->m_isMyPlayer = false;
    playerTwo->m_isCaptain = false;
    playerTwo->m_playerName = "MS Dhoni";
    playerTwo->m_imagePath = "msDhoni.jpg";
    
    Player* playerThree = new Player();
    playerThree->m_isMyPlayer = false;
    playerThree->m_isCaptain = false;
    playerThree->m_playerName = "Sachin Tendulkar";
    playerThree->m_imagePath = "sachinTendulkar.png";
    
    Player* playerFour = new Player();
    playerFour->m_isMyPlayer = false;
    playerFour->m_isCaptain = false;
    playerFour->m_playerName = "Virender Sehwag";
    playerFour->m_imagePath = "viru.jpg";
    
    Player* playerFive = new Player();
    playerFive->m_isMyPlayer = false;
    playerFive->m_isCaptain = false;
    playerFive->m_playerName = "Rahul Dravid";
    playerFive->m_imagePath = "rahulDravid.jpg";
    
    teamOne->m_players->push_back(playerOne);
    teamOne->m_players->push_back(playerTwo);
    teamOne->m_players->push_back(playerThree);
    teamOne->m_players->push_back(playerFour);
    teamOne->m_players->push_back(playerFive);
    
    
    Team* teamTwo = new Team();
    teamTwo->m_teamName = "ENGLAND";
    teamTwo->m_playersPerTeam = MAX_PLAYER;
    
    Player* player1 = new Player();
    player1->m_isMyPlayer = false;
    player1->m_isCaptain = true;
    player1->m_playerName = "Alaistar Cook";
    player1->m_imagePath = "cook.jpg";
    
    Player* player2 = new Player();
    player2->m_isMyPlayer = false;
    player2->m_isCaptain = false;
    player2->m_playerName = "Joe Root";
    player2->m_imagePath = "root.jpeg";
    
    Player* player3 = new Player();
    player3->m_isMyPlayer = false;
    player3->m_isCaptain = false;
    player3->m_playerName = "Naseer Hussain";
    player3->m_imagePath = "naseer.jpg";
    
    Player* player4 = new Player();
    player4->m_isMyPlayer = false;
    player4->m_isCaptain = true;
    player4->m_playerName = "Eoin Morgan";
    player4->m_imagePath = "morgan.jpg";
    
    Player* player5 = new Player();
    player5->m_isMyPlayer = false;
    player5->m_isCaptain = false;
    player5->m_playerName = "Andrew Flintoff";
    player5->m_imagePath = "flintoff.jpg";
    
    teamTwo->m_players->push_back(player1);
    teamTwo->m_players->push_back(player2);
    teamTwo->m_players->push_back(player3);
    teamTwo->m_players->push_back(player4);
    teamTwo->m_players->push_back(player5);
    
    m_gameData->m_teamVector->push_back(teamOne);
    m_gameData->m_teamVector->push_back(teamTwo);
}

void MainScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent){
    pEvent->stopPropagation();
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE){
        Director::getInstance()->end();
    }
}

void MainScene::onExit(){
    CCLOG("On exit Main Scene called");
    listener->release();
    Layer::onExit();
}
