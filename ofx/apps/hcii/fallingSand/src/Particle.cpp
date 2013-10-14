//
//  Particle.cpp
//  fallingSand
//
//  Created by Julia Schwarz on 9/3/13.
//
//

#include "Particle.h"
#include "constants.h"
#define GRAVITY -10
#define FALL_TIMEOUT_MS 2000
#define BORN_TIMEOUT_MS 0


void Particle::update() {
    long aliveTime = ofGetSystemTime() - bornTime;
    switch (state) {
        case BORN:
            if(aliveTime > BORN_TIMEOUT_MS) {
                state = FALLING;
                acceleration.y = GRAVITY;
            }
            break;
        case FALLING:
            if(aliveTime > FALL_TIMEOUT_MS) {
                state = INACTIVE;
            } else {
                // TODO: better floor detection
                if(location.y > FLOOR_THRESH) {
                    velocity += acceleration;
                    location += velocity;
                }
            }
            break;
        default:
            break;
            
    }
}

void Particle::addToMesh(ofMesh & mesh) {
    float pct;
    switch(state) {
        case INACTIVE:
            break;
        case BORN:
            mesh.addColor(ofColor(baseColor.getLightness()));
            mesh.addVertex(location);
            break;
        case FALLING:
            pct = 1 - ofMap(ofGetSystemTime() - bornTime, BORN_TIMEOUT_MS, FALL_TIMEOUT_MS, 0,1,true);
            mesh.addColor(ofColor(baseColor.getLightness() * pct));
            mesh.addVertex(location);
            break;
        default:
            break;
    }
}