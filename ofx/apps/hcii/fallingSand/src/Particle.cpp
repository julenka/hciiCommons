//
//  Particle.cpp
//  fallingSand
//
//  Created by Julia Schwarz on 9/3/13.
//
//

#include "Particle.h"
#include "constants.h"
#define GRAVITY -4
#define FALL_TIMEOUT_MS 4000
#define BORN_TIMEOUT_MS 600


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
    int gray;
    switch(state) {
        case INACTIVE:
            break;
        case BORN:
            mesh.addColor(ofColor(255,255,255));
            mesh.addVertex(location);
            break;
        case FALLING:
            pct = ((ofGetSystemTime() - bornTime) / (float)(FALL_TIMEOUT_MS - BORN_TIMEOUT_MS));
            if(pct > 1) pct = 1;
            gray = (int)(255 * (1 - pct));
            mesh.addColor(ofColor(gray));
            mesh.addVertex(location);
            break;
        default:
            break;
    }
}