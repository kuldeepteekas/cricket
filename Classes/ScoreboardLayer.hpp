//
//  ScoreboardLayer.hpp
//  cricket-mobile
//
//  Created by Kuldeep Teekas on 04/08/18.
//

#ifndef ScoreboardLayer_hpp
#define ScoreboardLayer_hpp

#include <stdio.h>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "constant.h"
#include "ui/UIWidget.h"
#include <exception>
#include "extensions/cocos-ext.h"
#include "commonClasses.h"

using namespace cocos2d::ui;
USING_NS_CC;

class ScoreboardLayer : public cocos2d::Layer,public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate {
    
    int m_tableSize;
    float tableWidth;
    float tableHeight;
    float rowHeight;
    
    GameData* m_gameData;
    
    cocos2d::extension::TableView* m_tableView;
    cocos2d::EventListenerTouchOneByOne *listener;
    cocos2d::EventListenerKeyboard *m_keyboardListener;
    
public:
    
    cocos2d::Size visibleSize;
    
    static ScoreboardLayer* createLayer();
    
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    
    void createBackground();
    void createTableView();
    void createCancelButton();
    void createFooterItems();
    
    void setGameData(GameData* data);
    std::string getStringForBattingStatus(BATTING_STATUS status);
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void cancelButtonCallback(Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(ScoreboardLayer);
    
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);
    virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx);
    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table);
    
    virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent);
    
};
#endif /* ScoreboardLayer_hpp */
