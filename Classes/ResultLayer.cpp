//
//  ResultLayer.cpp
//  cricket-mobile
//
//  Created by Kuldeep Teekas on 05/08/18.
//

#include "ResultLayer.hpp"
#include "AppDelegate.h"
#include "commonClasses.h"
#include "ScoreboardLayer.hpp"
#include "SplashScene.h"

using namespace cocos2d::extension;
using namespace std;

ResultLayer* ResultLayer::createLayer(){
    
    // 'layer' is an autorelease object
    auto layer = ResultLayer::create();
    // return the scene
    return layer;
}


// on "init" you need to initialize your instance
bool ResultLayer::init(){
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() ){
        return false;
    }
    m_gameData = nullptr;
    return true;
}

void ResultLayer::onEnter(){
    
    Layer::onEnter();
    visibleSize = Director::getInstance()->getVisibleSize();
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [=](Touch* touch, Event* event){
        return true;
    };
    listener->setSwallowTouches(true);
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    listener->retain();
    
    m_keyboardListener = EventListenerKeyboard::create();
    m_keyboardListener->onKeyReleased = CC_CALLBACK_2(ResultLayer::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_keyboardListener, this);
    
    createBackground();
    createCancelButton();
    createWinningText();
    createPlayerHolder();
    createMenuButtons();
    
}

void ResultLayer::createCancelButton() {
    
    Sprite* cancelIcon =  Sprite::create(IMAGE_PATH"cancel.png");
    
    MenuItemSprite* cancelItem = MenuItemSprite::create(cancelIcon, cancelIcon,CC_CALLBACK_1(ResultLayer::cancelButtonCallback, this));
    cancelItem->setPosition(Vec2(visibleSize.width - cancelIcon->getContentSize().width * 0.6,
                                 visibleSize.height - cancelIcon->getContentSize().height * 0.6));
    
    auto cancelMenu = Menu::create(cancelItem, NULL);
    cancelMenu->setPosition(Vec2(0,0));
    this->addChild(cancelMenu);
    
}


void ResultLayer::cancelButtonCallback(Ref* pSender) {
    CCLOG("cancelButtonCallback result layer");
    auto scene = SplashScene::createScene();
    Director::getInstance()->replaceScene(scene);
}

void ResultLayer::setGameData(GameData* data) {
    m_gameData = new GameData(data);
}

void ResultLayer::createMenuButtons() {
    
    Sprite* scoreboardIcon =  Sprite::create(IMAGE_PATH"scoreBoardBlack.png");
    MenuItemSprite* scoreCardItem = MenuItemSprite::create(scoreboardIcon, scoreboardIcon,CC_CALLBACK_1(ResultLayer::scoreBoardCallback, this));
    scoreCardItem->setPosition(Vec2(visibleSize.width * 0.92,
                                    visibleSize.height * 0.82));
    
    Sprite* homeIcon =  Sprite::create(IMAGE_PATH"home_black.png");
    MenuItemSprite* homeItem = MenuItemSprite::create(homeIcon,homeIcon,CC_CALLBACK_1(ResultLayer::cancelButtonCallback, this));
    homeItem->setPosition(Vec2(visibleSize.width * 0.08,
                                    visibleSize.height * 0.82));
    
    auto scoreMenu = Menu::create(homeItem,scoreCardItem, NULL);
    scoreMenu->setPosition(Vec2(0,0));
    this->addChild(scoreMenu);
    
    
}

void ResultLayer::scoreBoardCallback(Ref* pSender) {

    CCLOG("Scoreboard layer clicked");
    ScoreboardLayer* scoreBoard = ScoreboardLayer::createLayer();
    scoreBoard->setGameData(m_gameData);
    scoreBoard->setPosition(Vec2(0,0));
    this->addChild(scoreBoard);
}

void ResultLayer::createWinningText() {
    
    Label* welcomeText= Label::createWithTTF("PRESENTATION",FONT_HEADLINE,50);
    welcomeText->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.825));
    welcomeText->setColor(Color3B::BLACK);
    this->addChild(welcomeText);
    
    std::string teamName = "";
    if (m_gameData->m_teamVector->at(0)->m_isWinner) {
        teamName = m_gameData->m_teamVector->at(0)->m_teamName;
    }else if (m_gameData->m_teamVector->at(1)->m_isWinner){
        teamName = m_gameData->m_teamVector->at(1)->m_teamName;
    }
    
    teamName = "CONGRATULATIONS ! TEAM " + teamName;
    Label* congratsText= Label::createWithTTF(teamName,FONT_HEADLINE,50);
    congratsText->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.75));
    congratsText->setHorizontalAlignment(TextHAlignment::CENTER);
    congratsText->setColor(Color3B::BLACK);
    this->addChild(congratsText);
}

Player* ResultLayer::getManOfTheMatch() {
    
    Player* manOfTheMatch = nullptr;
    int indexOfMaxRun = -1;
    int maxRun = 0;
    int winningTeam = 0;
    if (m_gameData->m_teamVector->at(1)->m_isWinner)
        winningTeam = 1;
    
    for (int i=0; i <m_gameData->m_teamVector->at(winningTeam)->m_players->size(); i++) {
        if (m_gameData->m_teamVector->at(winningTeam)->m_players->at(i)->m_runs > maxRun) {
            indexOfMaxRun = i;
            maxRun = m_gameData->m_teamVector->at(winningTeam)->m_players->at(i)->m_runs;
        }
    }
    
    if (indexOfMaxRun != -1)
        manOfTheMatch = new Player(m_gameData->m_teamVector->at(winningTeam)->m_players->at(indexOfMaxRun));
    
    return manOfTheMatch;
    
}

Player* ResultLayer::getCaptainOfTeam(int teamIndex) {
 
    Player* captain = nullptr;
    int indexOfCaptain = -1;
    
    for (int i=0; i <m_gameData->m_teamVector->at(teamIndex)->m_players->size(); i++) {
        if (m_gameData->m_teamVector->at(teamIndex)->m_players->at(i)->m_isCaptain) {
            indexOfCaptain = i;
            break;
        }
    }
    
    if (indexOfCaptain != -1)
        captain = new Player(m_gameData->m_teamVector->at(teamIndex)->m_players->at(indexOfCaptain));
    
    return captain;
    
}

void ResultLayer::createPlayerHolder() {
    
    int winningTeam = 0;
    int losingTeam = 1;
    if (m_gameData->m_teamVector->at(1)->m_isWinner){
        winningTeam = 1;
        losingTeam = 0;
    }
    
    Player* manOfTheMatch = getManOfTheMatch();
    Player* winningCaptain = getCaptainOfTeam(winningTeam);
    Player* losingCaptain = getCaptainOfTeam(losingTeam);
    
    for (int i=0; i < 3; i++) {
        Scale9Sprite* playerBg = Scale9Sprite::create(IMAGE_PATH"playerBg.png");
        playerBg->setContentSize(Size(visibleSize.width * 0.25, visibleSize.height * 0.5));
        playerBg->setAnchorPoint(Vec2(0.5, 0));

        std::string imagePath = "";
        std::string playerName = "";
        
        if (i == 0) {
            
            if (winningCaptain != nullptr) {
                imagePath = winningCaptain->m_imagePath;
                playerName = winningCaptain->m_playerName;
                playerName = "WINNING CAPTAIN \n" + playerName;
            }
            playerBg->setPosition(Vec2(playerBg->getContentSize().width * 0.6,20));
            
        }else if (i == 1) {
            if (manOfTheMatch != nullptr) {
                imagePath = manOfTheMatch->m_imagePath;
                playerName = manOfTheMatch->m_playerName;
                std::string runs = std::to_string(manOfTheMatch->m_runs);
                playerName = "MAN OF THE MATCH \n" + playerName + " (" + runs + ")";
            }
            playerBg->setPosition(Vec2(visibleSize.width * 0.5,20));
        }else {
            if (losingCaptain != nullptr) {
                imagePath = losingCaptain->m_imagePath;
                playerName = losingCaptain->m_playerName;
                playerName = "LOSING CAPTAIN \n" + playerName;
            }
            playerBg->setPosition(Vec2(visibleSize.width - playerBg->getContentSize().width * 0.6,20));
        }
        this->addChild(playerBg);
        
        if (imagePath.empty())
            imagePath= "default.jpg";
        
        Sprite* m_teamOneCurrPlayerImage = Sprite::create(PLAYER_IMAGE_PATH + imagePath);
        Size ws = Size(playerBg->getContentSize().width * 0.85, playerBg->getContentSize().height * 0.85);
        Size maskSize = ws* 0.975;
        float radius = 30;
        int scaleBoundsX = playerBg->getContentSize().width * 0.85;
        int scaleBoundY = playerBg->getContentSize().height * 0.85;
        float scaleX = scaleBoundsX/m_teamOneCurrPlayerImage->getContentSize().width;
        float scaleY = scaleBoundY/m_teamOneCurrPlayerImage->getContentSize().height;
        m_teamOneCurrPlayerImage->setScaleX(scaleX);
        m_teamOneCurrPlayerImage->setScaleY(scaleY);
        
        // create masked image and position to center it on screen
        Node* clipNode = AppDelegate::getDelegate()->createRoundedRectMaskNode(maskSize, radius, 1.0f, 10);
        m_teamOneCurrPlayerImage->setAnchorPoint(Vec2(0,0));
        clipNode->setAnchorPoint(Vec2(0.5,0.5));
        clipNode->addChild(m_teamOneCurrPlayerImage);
        clipNode->setPosition(Vec2(playerBg->getContentSize().width * 0.075,
                                   playerBg->getContentSize().height * 0.1));
        playerBg->addChild(clipNode);
        
        if (!playerName.empty()) {
            Label* playerNameText= Label::createWithTTF(playerName,FONT_DOSIS,45);
            playerNameText->setAnchorPoint(Vec2(0.5,0));
            playerNameText->setPosition(Vec2(playerBg->getContentSize().width * 0.5,
                                             playerBg->getContentSize().height + 20));
            playerNameText->setAlignment(TextHAlignment::CENTER);
            playerNameText->setColor(Color3B::BLACK);
            playerBg->addChild(playerNameText);
        }
    }
}

void ResultLayer::createBackground(){
    
    LayerColor* bgLayer = LayerColor::create(Color4B(255,255,255,255), this->getContentSize().width, this->getContentSize().height);
    bgLayer->setPosition(Vec2(0,0));
    this->addChild(bgLayer);
    
    LayerColor* headerLayer = LayerColor::create(Color4B(27,38,49,255), visibleSize.width, visibleSize.height * 0.125);
    headerLayer->setPosition(Vec2(0, visibleSize.height - visibleSize.height * 0.125));
    this->addChild(headerLayer);
    
    Label* headerTeamText= Label::createWithTTF("",FONT_HEADLINE,60);
    headerTeamText->setPosition(Vec2(headerLayer->getContentSize().width * 0.5, headerLayer->getContentSize().height * 0.5));
    headerLayer->addChild(headerTeamText);
    
    if (headerTeamText != nullptr && m_gameData != nullptr) {
        std::string teamText = m_gameData->m_teamVector->at(0)->m_teamName + " " + "V" + " "
        + m_gameData->m_teamVector->at(1)->m_teamName;
        
        headerTeamText->setString(teamText);
    }
    
}

void ResultLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent){
    pEvent->stopPropagation();
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE){
        cancelButtonCallback(nullptr);
    }
}

void ResultLayer::onExit(){
    Layer::onExit();
    delete m_gameData;
    m_gameData = nullptr;
}
