#pragma once

#include "constants.h"
#include "ofMain.h"
#include "ofxKinect.h"
#include "Particle.h"
#include "Background.h"
using namespace std;

class testApp : public ofBaseApp {
public:

    Particle particles[PARTICLE_COUNT];
    deque<int> inactiveParticles;
    
    ofxKinect kinect;
	ofEasyCam easyCam;
    Background background;
    
    int cameraDirection;
    
    void drawDebugText();
    void makeParticleAt(const ofVec3f &pt, ofColor c);
    void updateParticles();
    void drawParticles();
	
    void drawFloor();
    ofColor getParticleColor();
    
    // ofx stuff below
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
    void keyReleased(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
private:
    ofTrueTypeFont verdana18;
    ofTrueTypeFont verdana48;
	
};
