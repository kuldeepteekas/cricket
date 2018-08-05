//
//  ResultLayer.hpp
//  cricket-mobile
//
//  Created by Kuldeep Teekas on 05/08/18.
//

#ifndef ResultLayer_hpp
#define ResultLayer_hpp

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

class ResultLayer : public cocos2d::Layer {
    
    GameData* m_gameData;
    cocos2d::EventListenerTouchOneByOne *listener;
    cocos2d::EventListenerKeyboard *m_keyboardListener;
    
public:
    
    cocos2d::Size visibleSize;
    
    static ResultLayer* createLayer();
    
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    
    void createBackground();
    void createCancelButton();
    void createWinningText();
    void createPlayerHolder();
    void createMenuButtons();
    
    Player* getManOfTheMatch();
    Player* getCaptainOfTeam(int teamIndex);
    
    
    void setGameData(GameData* data);
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void scoreBoardCallback(Ref* pSender);
    void cancelButtonCallback(Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(ResultLayer);
    virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event *pEvent);    
};
#endif /* ResultLayer_hpp */
