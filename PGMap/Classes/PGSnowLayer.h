//
//  PGSnowLayer.h
//  PGMap
//
//  Created by ryan on 13-12-27.
//
//

#ifndef __PGMap__PGSnowLayer__
#define __PGMap__PGSnowLayer__

#include "cocos2d.h"

USING_NS_CC;

class PGSnowLayer : public CCLayer {
public:
    PGSnowLayer();
    ~PGSnowLayer();
    static PGSnowLayer* create(int w,int h);
    bool init(int w,int h);
    void update(float dt);
    int getDepth() { return currentDepth; };
    int getMaxDepth() { return maxDepth; };
    void setDepth(int depth) { currentDepth = currentDepth < 10 ? 10 : depth; };
    class Particle {
    public:
        CCPoint position;
        float radius;
        float density;
        int depth;
    };
private:
    CCSpriteBatchNode* batchNode;
    CCArray* snows;
    Particle *particles;
    float angle;
    int maxCount;
    CCSize size;
    int currentDepth;
    int maxDepth;
};

#endif /* defined(__PGMap__PGSnowLayer__) */
