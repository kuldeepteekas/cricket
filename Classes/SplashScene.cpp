#include "SplashScene.h"
#include "constant.h"
#include "MainScene.hpp"

USING_NS_CC;

Scene* SplashScene::createScene(){
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    // 'layer' is an autorelease object
    auto layer = SplashScene::create();
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool SplashScene::init(){
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() ){
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    isSplash = true;
    
    return true;
}


void SplashScene::onEnter(){
    Layer::onEnter();
    Node::onEnter();
    
    LayerColor* layerBg = LayerColor::create(Color4B::WHITE, visibleSize.width, visibleSize.height);
    layerBg->setPosition(Vec2(0,0));
    this->addChild(layerBg);
    
    m_logo = Scale9Sprite::create("splash2.jpg");
    m_logo->setContentSize(Size(visibleSize.width, visibleSize.height));
    m_logo->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.5));
    m_logo->setOpacityModifyRGB(true);
    m_logo->setOpacity(0);
    this->addChild(m_logo);
    
    LayerColor* overlay = LayerColor::create(Color4B::BLACK, visibleSize.width, visibleSize.height);
    overlay->setPosition(Vec2(0,0));
    overlay->setOpacity(165);
    this->addChild(overlay);
    
    m_appName = Label::createWithTTF("QUICK CRICKET", FONT_HEADLINE, 100);
    m_appName->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.9));
    m_appName->setOpacityModifyRGB(true);
    m_appName->setOpacity(0);
    this->addChild(m_appName);
    
    runLogoAnimation();
}

void SplashScene::createPlayButton() {
    
    Scale9Sprite* bgSprite =  Scale9Sprite::create(IMAGE_PATH"rectangle.png");
    bgSprite->setContentSize(Size(visibleSize.width * 0.25, visibleSize.height * 0.1));
    
    Label* textLabel = Label::createWithTTF("PLAY NOW", FONT_DOSIS, 60);
    textLabel->setPosition(Vec2(bgSprite->getContentSize().width * 0.5, bgSprite->getContentSize().height * 0.5));
    textLabel->setColor(Color3B::BLACK);
    bgSprite->addChild(textLabel);
    
    MenuItemSprite* listItem = MenuItemSprite::create(bgSprite, bgSprite,CC_CALLBACK_1(SplashScene::playItemCallback, this));
    auto listMenu = Menu::create(listItem, NULL);
    listMenu->setPosition(Vec2(visibleSize.width * 0.5, visibleSize.height * 0.3));
    this->addChild(listMenu);
}

void SplashScene::playItemCallback(Ref* pSender){
    CCLOG("inside play callback");
    callGameScene();
}

void SplashScene::runLogoAnimation(){
    
    CallFunc* callFun1 = CallFunc::create([=]{
        m_logo->runAction(FadeIn::create(4));
    });

    CallFunc* callFun2 = CallFunc::create([=]{
        m_appName->runAction(FadeIn::create(4));
        
    });
    
    CallFunc* callFun3 = CallFunc::create([=]{
        isSplash = false;
        createPlayButton();
        
    });

    //delay is implemented so as the current implementation will complete and then the main scene will populate
    this->runAction(Spawn::create(callFun1, callFun2, NULL));
    this->runAction(Sequence::create(DelayTime::create(6),callFun3, NULL));
    
}

void SplashScene::callGameScene() {
    auto mainScene = MainScene::createScene();
    Director::getInstance()->replaceScene(mainScene);
}

void SplashScene::onExit(){
    Layer::onExit();
}

void SplashScene::menuCloseCallback(Ref* pSender){
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
