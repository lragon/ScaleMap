//
//  PGSnowLayer.cpp
//  PGMap
//
//  Created by ryan on 13-12-27.
//
//

#include "PGSnowLayer.h"

PGSnowLayer::PGSnowLayer()
{
    
}

PGSnowLayer::~PGSnowLayer()
{
    
}

PGSnowLayer* PGSnowLayer::create(int w, int h)
{
    PGSnowLayer* ref = new PGSnowLayer;
    if (ref && ref->init(w, h)) {
        ref->autorelease();
        return ref;
    }else {
        delete ref;
        ref = NULL;
        return NULL;
    }
}

bool PGSnowLayer::init(int w, int h)
{
    srand(time(NULL));
    const char* fileName = "snow.png";
    batchNode = CCSpriteBatchNode::create(fileName);
    maxCount = 200;
    angle = 0;
    snows = CCArray::createWithCapacity(maxCount);
    snows->retain();
    particles = (Particle*)malloc(sizeof(Particle)*maxCount);
    size = CCSize(w, h);
    maxDepth = currentDepth = 50;
    for (int i=0; i<maxCount; i++) {
        CCSprite* snow = CCSprite::create(fileName);
        particles[i].position.x = CCRANDOM_0_1()*size.width;
        particles[i].position.y = CCRANDOM_0_1()*size.height;
        if (i%7<2) {
            particles[i].depth = 8;
        }else if (i%7<4) {
            particles[i].depth = 16;
        }else {
            particles[i].depth = i%7*8;
        }
        particles[i].radius = CCRANDOM_0_1()*4+1;
        particles[i].density = CCRANDOM_0_1()*M_PI/2;
        snow->setScale(particles[i].depth*1.0/currentDepth);
        snow->setPosition(particles[i].position);
        snow->setOpacity(255);
        batchNode->addChild(snow);
        snows->addObject(snow);
    }
    this->addChild(batchNode);
    this->scheduleUpdate();
    return true;
}

void PGSnowLayer::update(float dt)
{
    angle += dt;
    for (int i=0; i<maxCount; i++) {
        Particle *p = &particles[i];
        p->position.y -= cosf(angle+p->density) + 1 + p->depth/2;
        p->position.x += sinf(angle+p->density)*p->depth/2;
        if (p->position.x>size.width+20 || p->position.x < -20 || p->position.y<0) {
            if (i%5>0) {
                p->position.x = CCRANDOM_0_1()*size.width;
                p->position.y = size.height;
            }
            else {
                if (sinf(angle)>0) {
                    p->position.x = -5;
                    p->position.y = CCRANDOM_0_1()*size.height;
                }else {
                    p->position.x = size.width+5;
                    p->position.y = CCRANDOM_0_1()*size.height;
                }
            }
        }
        CCSprite* snow = (CCSprite*)snows->objectAtIndex(i);
        snow->setPosition(particles[i].position);
        if (p->depth<currentDepth) {
            snow->setVisible(true);
            snow->setScale(p->depth*1.0/currentDepth);
        }
        else {
            snow->setVisible(false);
        }
    }
}



