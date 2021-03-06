//
//  Particle.h
//  fallingSand
//
//  Created by Julia Schwarz on 9/3/13.
//
//

#ifndef __fallingSand__Particle__
#define __fallingSand__Particle__

#include <iostream>
#include "ofMain.h"

class Particle {
public:
    ofVec3f location;
    ofVec3f velocity;
    ofVec3f acceleration;
    ofColor baseColor;
    ofColor curColor;
    unsigned long long bornTime;
    enum particle_state {
        INACTIVE,
        BORN,
        FALLING,
        
        PARTICLE_STATE_COUNT,
    };
    enum particle_state state;
    void update(long systemTime);
    enum particle_state getState() { return state; }
    void addToMesh(ofMesh & mesh, unsigned long long systemTime);
};

#endif /* defined(__fallingSand__Particle__) */
