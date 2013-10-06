#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "Particle.h"
#define PARTICLE_COUNT 60000

#define DEPTH_MAP_SIZE 307200
using namespace std;

class testApp : public ofBaseApp {
public:
    float backgroundMean[DEPTH_MAP_SIZE];
    float backgroundStdev[DEPTH_MAP_SIZE];
    int backgroundCount[DEPTH_MAP_SIZE];
    int bgFrameCount = 0;
    Particle particles[PARTICLE_COUNT];
    deque<int> inactiveParticles;
    
    ofxKinect kinect;
	ofEasyCam easyCam;
    ofCamera camera;
    ofVec3f center;
    
    void updateBackground();
    void drawDebugInfo();
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
