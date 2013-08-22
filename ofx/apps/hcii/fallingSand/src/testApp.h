#pragma once

#include "ofMain.h"
#include "ofxKinect.h"

#define PARTICLE_COUNT 30000

#define DEPTH_MAP_SIZE 307200
using namespace std;

class testApp : public ofBaseApp {
public:
    enum particle_state {
        INACTIVE,
        BORN,
        FALLING,
        
        PARTICLE_STATE_COUNT,
    };
	struct Particle {
        float x;
        float y;
        float z;
        float vx;
        float vy;
        float vz;
        float ax;
        float ay;
        float az;
        unsigned long long bornTime;
        enum particle_state state;
    };
    
    bool bgInit = true;
    ofVec3f background[DEPTH_MAP_SIZE];
    struct Particle particles[PARTICLE_COUNT];
    deque<int> inactiveParticles;
    
    ofxKinect kinect;
	ofEasyCam easyCam;
    
    void updateBackground();
    void drawDebugText();
    void makeParticleAt(const ofVec3f &pt);
    void updateParticles();
    void drawParticles();
	
    // ofx stuff below
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	
};
