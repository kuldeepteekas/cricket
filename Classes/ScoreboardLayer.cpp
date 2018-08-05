//
//  ScoreboardLayer.cpp
//  cricket-mobile
//
//  Created by Kuldeep Teekas on 04/08/18.
//

#include "ScoreboardLayer.hpp"
#include "AppDelegate.h"
#include "commonClasses.h"

using namespace cocos2d::extension;
using namespace std;

#define HEADER_LABEL_TAG                    213
#define CELL_BG_TAG                         214


ScoreboardLayer* ScoreboardLayer::createLayer(){
    
    // 'layer' is an autorelease object
    auto layer = ScoreboardLayer::create();
    // return the scene
    return layer;
}


// on "init" you need to initialize your instance
bool ScoreboardLayer::init(){
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() ){
        return false;
    }
    m_gameData = nullptr;
    return true;
}

void ScoreboardLayer::onEnter(){
    
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
    m_keyboardListener->onKeyReleased = CC_CALLBACK_2(ScoreboardLayer::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_keyboardListener, this);
    
    createBackground();
    createCancelButton();
    
    if (m_gameData != nullptr) {
         createTableView();
    }
    
    createFooterItems();
    
}

void ScoreboardLayer::createTableView(){
    
    tableWidth = this->getContentSize().width;
    tableHeight = this->getContentSize().height * 0.75;
    
    m_tableView = cocos2d::extension::TableView::create(this, Size(tableWidth,tableHeight));
    m_tableView->setDirection(cocos2d::extension::TableView::Direction::VERTICAL);
    m_tableView->setPosition(Vec2(0,visibleSize.height * 0.1));
    m_tableView->setDelegate(this);
    m_tableView->setVerticalFillOrder(cocos2d::extension::TableView::VerticalFillOrder::TOP_DOWN);
    //    m_tableView->setBounceable(false);
    //    m_tableView->setContentSize(Size(tableWidth,tableHeight));
    this->addChild(m_tableView);
    
}

Size ScoreboardLayer::tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx) {
    rowHeight = 120;
    return Size(tableWidth, rowHeight);
}

void ScoreboardLayer::tableCellTouched(TableView *table, TableViewCell *cell){
    
    CCLOG("CELL %zd ", cell->getIdx());
    int idx = (int)cell->getIdx();
}

TableViewCell* ScoreboardLayer::tableCellAtIndex(TableView *table, ssize_t idx){
    
    TableViewCell *cell = table->dequeueCell();
    if (cell){
        cell->removeAllChildrenWithCleanup(true);
    }else{
        cell = new TableViewCell();
        cell->autorelease();
    }

    DrawNode* line = DrawNode::create();
    line->drawLine(Vec2(tableWidth * 0.5, rowHeight), Vec2(tableWidth * 0.5, 0), Color4F::BLACK);
    cell->addChild(line);
    
    std::string serial = std::to_string(idx + 1);
    
    for (int i=0; i < m_gameData->m_teamVector->size(); i++) {
        
        std::string playerName = "";
        int runScored;
        int ballFaced;
        BATTING_STATUS battingStatus = BATSMAN_BATTING;
        
        if (idx == m_gameData->m_maxPlayer) {
            playerName = "Extras";
            runScored = m_gameData->m_teamVector->at(i)->m_extraRun;
        }else {
            playerName = m_gameData->m_teamVector->at(i)->m_players->at(idx)->m_playerName;
            battingStatus = m_gameData->m_teamVector->at(i)->m_players->at(idx)->m_battingStatus;
            runScored = m_gameData->m_teamVector->at(i)->m_players->at(idx)->m_runs;
            ballFaced = m_gameData->m_teamVector->at(i)->m_players->at(idx)->m_ballsPlayed;
        }
        
        float basePosX = 0;
        if (i == 1) {
            basePosX = tableWidth * 0.51;
        }
        
        Color4B bgColor = Color4B(255,255,255,255);
        if (idx == m_gameData->m_maxPlayer)
            bgColor = Color4B(220,118,51,255);
        else if (battingStatus == BATSMAN_OUT)
            bgColor = Color4B(255,255,255,200);
        else if (battingStatus == BATSMAN_BATTING)
            bgColor = Color4B(169,223,191,255);
        
        LayerColor* cellBg = LayerColor::create(bgColor, tableWidth * 0.5, rowHeight - 7);
        cellBg->setPosition(Vec2(basePosX + 0,0));
        cell->addChild(cellBg);
        
        if (idx != m_gameData->m_maxPlayer) {
            
            Sprite* numberBg = Sprite::create("blueCircle.png");
            numberBg->setAnchorPoint(Vec2(0, 0.5));
            numberBg->setPosition(Vec2(20, cellBg->getContentSize().height * 0.5));
            cellBg->addChild(numberBg);
            
            Label* serialNo = Label::createWithTTF(serial, FONT_DOSIS, 30);
            serialNo->setAnchorPoint(Vec2(0.5, 0.5));
            serialNo->setColor(Color3B::WHITE);
            serialNo->setPosition(Vec2(numberBg->getContentSize().width * 0.5, numberBg->getContentSize().height * 0.5));
            numberBg->addChild(serialNo);
            
            std::string statusText = getStringForBattingStatus(battingStatus);
            Label* battingStatusText= Label::createWithTTF(statusText,FONT_HEADLINE,45);
            battingStatusText->setAnchorPoint(Vec2(0, 0.5));
            battingStatusText->setColor(Color3B::BLACK);
            battingStatusText->setPosition(Vec2(tableWidth * 0.285, rowHeight * 0.5));
            cellBg->addChild(battingStatusText);
        }
        
        Label* playerNameText= Label::createWithTTF(playerName,FONT_HEADLINE,45);
        playerNameText->setAnchorPoint(Vec2(0, 0.5));
        playerNameText->setColor(Color3B::BLACK);
        playerNameText->setPosition(Vec2(80, rowHeight * 0.5));
        cellBg->addChild(playerNameText);
        
        if (battingStatus != BATSMAN_YET_TO_BAT){
            Label* runScoredText= Label::createWithTTF(std::to_string(runScored),FONT_HEADLINE,42);
            runScoredText->setAnchorPoint(Vec2(1, 0.5));
            runScoredText->setColor(Color3B::BLACK);
            runScoredText->setPosition(Vec2(tableWidth * 0.43, rowHeight * 0.5));
            cellBg->addChild(runScoredText);
            
            if (idx != m_gameData->m_maxPlayer) {
                std::string text = "(" + std::to_string(ballFaced) + ")";
                Label* ballFacedText= Label::createWithTTF(text,FONT_HEADLINE,40);
                ballFacedText->setAnchorPoint(Vec2(1, 0.5));
                ballFacedText->setColor(Color3B::BLACK);
                ballFacedText->setPosition(Vec2(tableWidth * 0.48, rowHeight * 0.5));
                cellBg->addChild(ballFacedText);
            }
        }
    }
    
    return cell;
    
}

std::string ScoreboardLayer::getStringForBattingStatus(BATTING_STATUS status) {
    
    std::string result = "";
    if (status == BATSMAN_OUT) {
        result = "Out";
    }else if (status == BATSMAN_BATTING) {
        result = "Batting";
    }else if (status == BATSMAN_YET_TO_BAT) {
        result = "Still to Bat";
    }
    
    return result;
}
ssize_t ScoreboardLayer::numberOfCellsInTableView(TableView *table){
    int size = (int)m_gameData->m_maxPlayer + 1;
    return size;
}

void ScoreboardLayer::createCancelButton() {
    
    Sprite* cancelIcon =  Sprite::create(IMAGE_PATH"cancel.png");
    
    MenuItemSprite* cancelItem = MenuItemSprite::create(cancelIcon, cancelIcon,CC_CALLBACK_1(ScoreboardLayer::cancelButtonCallback, this));
    cancelItem->setPosition(Vec2(visibleSize.width - cancelIcon->getContentSize().width * 0.6,
                                    visibleSize.height - cancelIcon->getContentSize().height * 0.6));
    
    auto cancelMenu = Menu::create(cancelItem, NULL);
    cancelMenu->setPosition(Vec2(0,0));
    this->addChild(cancelMenu);
    
}

void ScoreboardLayer::cancelButtonCallback(Ref* pSender) {
    CCLOG("Scoreboard layer clicked");
    this->removeFromParentAndCleanup(true);
}

void ScoreboardLayer::setGameData(GameData* data) {
    m_gameData = new GameData(data);
}

void ScoreboardLayer::createBackground(){
    
    LayerColor* bgLayer = LayerColor::create(Color4B(0,0,0,247), this->getContentSize().width, this->getContentSize().height);
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

void ScoreboardLayer::createFooterItems() {
    
    LayerColor* m_bottomLayer = LayerColor::create(Color4B(27,38,49,255), visibleSize.width, visibleSize.height * 0.1);
    m_bottomLayer->setPosition(Vec2(0, 0));
    this->addChild(m_bottomLayer);
    
    Label* overTextLabel = Label::createWithTTF("OVERS:",FONT_HEADLINE, 50);
    overTextLabel->setAnchorPoint(Vec2(1, 0.5));
    overTextLabel->setPosition(Vec2(m_bottomLayer->getContentSize().width * 0.5 - overTextLabel->getContentSize().width * 0.05,
                                    m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(overTextLabel);
    
    
    Label* m_overLabel = Label::createWithTTF("",FONT_HEADLINE,50);
    m_overLabel->setAnchorPoint(Vec2(0, 0.5));
    m_overLabel->setPosition(Vec2(m_bottomLayer->getContentSize().width * 0.5,
                                  m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_overLabel);
    
    Label* m_teamOneNameFooter = Label::createWithTTF("",FONT_HEADLINE, 40);
    m_teamOneNameFooter->setAnchorPoint(Vec2(0, 0.5));
    m_teamOneNameFooter->setPosition(Vec2(20,
                                          m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_teamOneNameFooter);
    
    Label* m_teamTwoNameFooter = Label::createWithTTF("",FONT_HEADLINE, 40);
    m_teamTwoNameFooter->setAnchorPoint(Vec2(1, 0.5));
    m_teamTwoNameFooter->setPosition(Vec2(m_bottomLayer->getContentSize().width - 20,
                                          m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_teamTwoNameFooter);
    
    
    Label* m_teamOneScoreLabel = Label::createWithTTF("",FONT_HEADLINE, 50);
    m_teamOneScoreLabel->setAnchorPoint(Vec2(0, 0.5));
    m_teamOneScoreLabel->setPosition(Vec2(m_teamOneNameFooter->getPositionX() + m_teamOneNameFooter->getContentSize().width * 1.1,
                                          m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_teamOneScoreLabel);
    
    Label* m_teamTwoScoreLabel = Label::createWithTTF("",FONT_HEADLINE, 50);
    m_teamTwoScoreLabel->setAnchorPoint(Vec2(1, 0.5));
    m_teamTwoScoreLabel->setPosition(Vec2(m_teamTwoNameFooter->getPositionX() - m_teamTwoNameFooter->getContentSize().width * 1.1,
                                          m_bottomLayer->getContentSize().height * 0.5));
    m_bottomLayer->addChild(m_teamTwoScoreLabel);
    
    
    m_teamOneNameFooter->setString(m_gameData->m_teamVector->at(0)->m_teamName);
    m_teamTwoNameFooter->setString(m_gameData->m_teamVector->at(1)->m_teamName);
    
    int totalOvers = m_gameData->m_totalOvers;
    int currOver = m_gameData->m_currOver;
    int balls = m_gameData->m_numOfBall;
    
    std::string overText = std::to_string(currOver) + "." + std::to_string(balls) + "/" + std::to_string(totalOvers);
    m_overLabel->setString(overText);
    
    int totalRunsOne = m_gameData->m_teamVector->at(0)->m_teamScore;
    int wicketDownOne = m_gameData->m_teamVector->at(0)->m_wicketDown;
    
    std::string scoreTextOne = std::to_string(totalRunsOne) + "/" + std::to_string(wicketDownOne);
    m_teamOneScoreLabel->setString(scoreTextOne);
    
    int totalRunsTwo = m_gameData->m_teamVector->at(1)->m_teamScore;
    int wicketDownTwo = m_gameData->m_teamVector->at(1)->m_wicketDown;
    
    std::string scoreTextTwo = std::to_string(totalRunsTwo) + "/" + std::to_string(wicketDownTwo);
    m_teamTwoScoreLabel->setString(scoreTextTwo);
    
    m_teamOneScoreLabel->setPosition(Vec2(m_teamOneNameFooter->getPositionX() + m_teamOneNameFooter->getContentSize().width * 1.1,
                                          m_bottomLayer->getContentSize().height * 0.5));
    
    m_teamTwoScoreLabel->setPosition(Vec2(m_teamTwoNameFooter->getPositionX() - m_teamTwoNameFooter->getContentSize().width * 1.1,
                                          m_bottomLayer->getContentSize().height * 0.5));
    
}

void ScoreboardLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent){
    pEvent->stopPropagation();
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE){
        cancelButtonCallback(nullptr);
    }
}

void ScoreboardLayer::onExit(){
    Layer::onExit();
    delete m_gameData;
    m_gameData = nullptr;
}
