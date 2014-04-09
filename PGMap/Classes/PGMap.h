//
//  PGMap.h
//  PGMap
//
//  Created by ryan on 13-10-8.
//
//

#ifndef __PGMap__PGMap__
#define __PGMap__PGMap__

#include "cocos2d.h"
#include "PGSnowLayer.h"

USING_NS_CC;
using namespace std;

class PGMap : public CCLayer {
public:
    PGMap();
    ~PGMap();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    CREATE_FUNC(PGMap);
    virtual void registerWithTouchDispatcher(void);
    bool init();
    void setScale(float scale);
    void applyFrameLimits();
    void fixScale();
    void deaccelerateScrolling(float dt);
    void scaleBack(float dt);
    CCArray* touches;
    bool isdraging;
    bool isPinch;
    CCPoint scrollDistance;
    float distance;
    CCPoint lastMidPoint;
    CCPoint mapCenter;
    float lastScale;
    float minScale;
    PGSnowLayer* snowLayer;
};

#endif /* defined(__PGMap__PGMap__) */
