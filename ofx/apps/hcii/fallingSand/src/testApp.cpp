#include "testApp.h"


void testApp::makeParticleAt(const ofVec3f &pt) {
    if(inactiveParticles.size() == 0) {
        return;
    }
    int idx = inactiveParticles.front();
    Particle *p = &particles[idx];
    p->location.x = pt.x + ofRandom( -100, 100);
    p->location.y = pt.y + ofRandom( -100, 100);
    p->location.z = pt.z + ofRandom( -100, 100);
    p->velocity = ofVec3f(0);
    p->acceleration = ofVec3f(0);
    p->state = Particle::BORN;
    p->bornTime = ofGetSystemTime();
    inactiveParticles.pop_front();
}

void testApp::drawParticles() {
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
    for(int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].addToMesh(mesh);
    }
	glPointSize(2);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(-1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}

void testApp::updateParticles() {
    for(int i = 0; i < PARTICLE_COUNT; i++) {
        Particle::particle_state oldState = particles[i].state;
        particles[i].update();
        Particle::particle_state newState = particles[i].state;
        if(particles[i].state == Particle::INACTIVE && oldState == Particle::FALLING)
            inactiveParticles.push_back(i);
    }
}

void testApp::drawDebugText() {
    ofPushStyle();
    char buf[100];
    sprintf(buf, "inactive size: %ld\nfps: %f\nbg: %d/%d", inactiveParticles.size(), ofGetFrameRate(), background.getBgFrameCount(), NUM_BG_FRAMES);
    ofSetColor(255, 0, 255);
    ofDrawBitmapString(buf, ofPoint(10,10));
    ofPopStyle();
}

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
    background.setup();
    
    for(int i = 0; i < PARTICLE_COUNT; i++) {
        inactiveParticles.push_back(i);
        particles[i].state = Particle::INACTIVE;
    }
    
	kinect.setRegistration(true);
	kinect.init(false, false); // disable video image (faster fps)
	kinect.open();		// opens first available kinect
	
	ofSetFrameRate(60);
		
}

//--------------------------------------------------------------
void testApp::update() {
	kinect.update();
    updateParticles();
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
        if(background.getBgFrameCount() < NUM_BG_FRAMES) {
            background.update(kinect);
            return;
        }
        background.update(kinect);
        for(int y = 0; y < DEPTH_MAP_HEIGHT; y++) {
            for(int x = 0; x < DEPTH_MAP_WIDTH; x++) {
                int i = y * DEPTH_MAP_WIDTH + x;
                ofVec3f p = kinect.getWorldCoordinateAt(x, y);
                if(background.isBackground(kinect, x, y))continue;
                if(ofRandom(0, RANDOM_THRESH) <= 1) {
                    makeParticleAt(p);
                }
            }
        }
	}

}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackgroundGradient(ofColor(100,100,100), ofColor::black, OF_GRADIENT_CIRCULAR);
    easyCam.begin();
    drawParticles();
    easyCam.end();
    kinect.drawDepth(0, 0, 160, 120);
    background.drawMeanBg(0, 130, 160, 120);
    drawDebugText();
}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
    }
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
