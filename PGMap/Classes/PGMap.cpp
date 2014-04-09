//
//  PGMap.cpp
//  PGMap
//
//  Created by ryan on 13-10-8.
//
//

#include "PGMap.h"
#include "PGSnowLayer.h"

//下面的参数可以根据需要调整
#define MAP_SCROLL_DEACCEL_RATE  0.82f  //滑动摩擦系数，越小摩擦力越大
#define MAP_SCROLL_DEACCEL_DIST  1.0f   //最小滚动距离
#define MAP_SCALE_MAX_FLEX 1.6f         //缩放最大比例(手指在屏幕上时)
#define MAP_SCALE_MAX 1.0f              //缩放最大比例(手指离开屏幕时)

PGMap::PGMap()
{
    
}

PGMap::~PGMap()
{
    
}

bool PGMap::init()
{
    CCLayer::init();

    CCSprite* part1 = CCSprite::create("map.png");
    part1->setScale(2.0);
    part1->setAnchorPoint(ccp(0, 0));
    
    CCSize partSize = part1->boundingBox().size;
    part1->setPosition(ccp(0, 0));
    
    this->addChild(part1);
    
    CCSize mapSize = CCSizeMake(partSize.width, partSize.height);
    this->setContentSize(mapSize);
    
    touches = CCArray::createWithCapacity(2);
    touches->retain();
    isdraging = false;
    isPinch = false;
    scrollDistance = ccp(0, 0);
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    float minScaleX = winSize.width/mapSize.width;
    float minScaleY = winSize.height/mapSize.height;
    minScale = MAX(minScaleX, minScaleY);
    
    this->setTouchEnabled(true);
    this->ignoreAnchorPointForPosition(false);
    
    snowLayer = PGSnowLayer::create(partSize.width, partSize.height);
    this->addChild(snowLayer);
    
    return true;
}

void PGMap::setScale(float scale)
{
    CCLayer::setScale(scale);
    snowLayer->setDepth(snowLayer->getMaxDepth()*(1-scale/MAP_SCALE_MAX_FLEX));
}

void PGMap::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

bool PGMap::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    if (!touches->containsObject(pTouch) && touches->count()<2) {
        touches->addObject(pTouch);
        if (touches->count()==1) {
            //移动
            scrollDistance = ccp(0, 0);
        }else if (touches->count()==2) {
            //缩放
            CCTouch *touch1 = (CCTouch*)touches->objectAtIndex(0);
            CCTouch *touch2 = (CCTouch*)touches->objectAtIndex(1);
            CCPoint point1 = touch1->getLocation();
            CCPoint point2 = touch2->getLocation();
            distance = ccpDistance(point1, point2);
            lastScale = this->getScale();
            lastMidPoint = ccpAdd(point1, ccpMult(ccpSub(point2, point1), 0.5));
            CCSize size = this->getContentSize();
            mapCenter = this->convertToNodeSpace(lastMidPoint);
            if (mapCenter.x<0) {
                mapCenter.x = 0;
            }
            if (mapCenter.y<0) {
                mapCenter.y = 0;
            }
            if (mapCenter.x>size.width) {
                mapCenter.x = size.width;
            }
            if (mapCenter.y>size.height) {
                mapCenter.y = size.height;
            }
            this->setAnchorPoint(ccp(mapCenter.x/size.width, mapCenter.y/size.height));
            this->setPosition(lastMidPoint);
            isPinch = true;
        }
        isdraging = true;
    }
    return true;
}

void PGMap::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    if (touches->containsObject(pTouch)) {
        if (touches->count()==1) {
            scrollDistance = pTouch->getDelta();
            CCPoint pos = ccpAdd(this->getPosition(), scrollDistance);
            this->setPosition(pos);
        }else if (touches->count()==2) {
            CCTouch *touch1 = (CCTouch*)touches->objectAtIndex(0);
            CCTouch *touch2 = (CCTouch*)touches->objectAtIndex(1);
            CCPoint point1 = touch1->getLocation();
            CCPoint point2 = touch2->getLocation();
            float newdis = ccpDistance(point1, point2);
            float scalediff = newdis/distance;
            this->setScale(scalediff*lastScale);
            
            CCPoint midPoint = ccpAdd(point1, ccpMult(ccpSub(point2, point1), 0.5));
            this->setPosition(midPoint);
        }
        this->applyFrameLimits();
    }
}

void PGMap::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    if (touches->containsObject(pTouch)) {
        if (touches->count()==2) {
            lastScale = this->getScale();
            this->fixScale();
            isPinch = false;
        }else if (touches->count()==1) {
            isdraging = false;
            this->schedule(schedule_selector(PGMap::deaccelerateScrolling));
        }
        touches->removeObject(pTouch);
    }
}

void PGMap::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    this->ccTouchEnded(pTouch, pEvent);
}

void PGMap::fixScale()
{
    float scale = this->getScale();
    if (scale>MAP_SCALE_MAX) {
        this->schedule(schedule_selector(PGMap::scaleBack));
    }
}

void PGMap::applyFrameLimits()
{
    CCPoint position = this->getPosition();
    float scale = this->getScale();
    CCSize size = this->getContentSize();
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    if (scale>MAP_SCALE_MAX_FLEX) {
        scale = MAP_SCALE_MAX_FLEX;
        this->setScale(scale);
    }
    if (scale<minScale) {
        scale = minScale;
        this->setScale(minScale);
    }
    CCRect rect = this->boundingBox();
    CCPoint leftbom = rect.origin;
    CCPoint righttop = ccp(leftbom.x+rect.size.width, leftbom.y+rect.size.height);
    if (leftbom.x>0) {
        this->setPositionX(position.x-leftbom.x);
    }
    if (leftbom.y>0) {
        this->setPositionY(position.y-leftbom.y);
    }
    if (righttop.x<winSize.width) {
        this->setPositionX(position.x+(winSize.width-righttop.x));
    }
    if (righttop.y<winSize.height) {
        this->setPositionY(position.y+(winSize.height-righttop.y));
    }
}

void PGMap::deaccelerateScrolling(float dt)
{
    if (isdraging) {
        this->unschedule(schedule_selector(PGMap::deaccelerateScrolling));
        return;
    }
    this->setPosition(ccpAdd(this->getPosition(), scrollDistance));
    scrollDistance = ccpMult(scrollDistance, MAP_SCROLL_DEACCEL_RATE);
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCRect rect = this->boundingBox();
    CCPoint leftbom = rect.origin;
    CCPoint righttop = ccp(leftbom.x+rect.size.width, leftbom.y+rect.size.height);
    if (leftbom.x>0 || righttop.x<winSize.width || fabsf(scrollDistance.x) <= MAP_SCROLL_DEACCEL_DIST) {
        scrollDistance.x = 0;
    }
    if (leftbom.y>0 || righttop.y<winSize.height || fabsf(scrollDistance.y) <= MAP_SCROLL_DEACCEL_DIST) {
        scrollDistance.y = 0;
    }
    this->applyFrameLimits();
    if (scrollDistance.equals(CCPointZero)) {
        this->unschedule(schedule_selector(PGMap::deaccelerateScrolling));
    }
}

void PGMap::scaleBack(float dt)
{
    if (isPinch) {
        this->unschedule(schedule_selector(PGMap::scaleBack));
        return;
    }
    float newscale = this->getScale()+(MAP_SCALE_MAX-this->getScale())*dt*3;
    if (newscale>MAP_SCALE_MAX) {
        this->setScale(newscale);
    } else {
        this->setScale(MAP_SCALE_MAX);
        this->unschedule(schedule_selector(PGMap::scaleBack));
    }
    this->applyFrameLimits();
}

