#include "testApp.h"

#define DEPTH_MAP_WIDTH 640
#define DEPTH_MAP_HEIGHT 480
#define BORN_TIMEOUT_MS 100
#define FALL_TIMEOUT_MS 4000
#define GRAVITY 5
#define RANDOM_THRESH 500

void testApp::makeParticleAt(const ofVec3f &pt) {
    if(inactiveParticles.size() == 0) {
        ofLog(OF_LOG_WARNING, "makeParticlesAt inactiveParticles queue is empty!");
        return;
    }
    int idx = inactiveParticles.front();
    Particle *p = &particles[idx];
    p->x = pt.x;
    p->y = pt.y;
    p->z = pt.z;
    p->vx = 0;
    p->vy = 0;
    p->vz = 0;
    p->ax = 0;
    p->ay = 0;
    p->az = 0;
    p->state = BORN;
    p->bornTime = ofGetSystemTime();
    inactiveParticles.pop_front();
}

void testApp::drawParticles() {
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
    for(int i = 0; i < PARTICLE_COUNT; i++) {
        Particle *p = &particles[i];
        switch(p->state) {
            case INACTIVE:
                break;
            case BORN:
                mesh.addColor(ofColor(255,255,255));
                mesh.addVertex(ofVec3f(p->x, p->y, p->z));
                break;
            case FALLING:
                mesh.addColor(ofColor(100,100,100));
                mesh.addVertex(ofVec3f(p->x, p->y, p->z));
                break;
            default:
                break;
        }
    }
	glPointSize(2);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}

void testApp::updateParticles() {
    for(int i = 0; i < PARTICLE_COUNT; i++) {
        Particle *p = &particles[i];
        long aliveTime = ofGetSystemTime() - p->bornTime;
        switch (p->state) {
            case BORN:
                if(aliveTime > BORN_TIMEOUT_MS) {
                    p->state = FALLING;
                    p->ay = GRAVITY;
                }
                break;
            case FALLING:
                if(aliveTime > FALL_TIMEOUT_MS) {
                    p->state = INACTIVE;
                    inactiveParticles.push_back(i);
                } else {
                    p->vx += p ->ax;
                    p->vy += p->ay;
                    p->vz += p->az;
                    p->x += p->vx;
                    p->y += p->vy;
                    p->z += p->vz;
                }
                break;
            default:
                break;
                
        }
    }
}

void testApp::drawDebugText() {
    char buf[100];
    sprintf(buf, "inactive size: %ld", inactiveParticles.size());
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(buf, ofPoint(10,10));
}

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
    
    for(int i = 0; i < PARTICLE_COUNT; i++) {
        inactiveParticles.push_back(i);
        particles[i].state = INACTIVE;
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
        for(int y = 0; y < DEPTH_MAP_HEIGHT; y++) {
            for(int x = 0; x < DEPTH_MAP_WIDTH; x++) {
                if(ofRandom(0, RANDOM_THRESH) <= 1) {
                    makeParticleAt(kinect.getWorldCoordinateAt(x, y));
                }
            }
        }
	}
	
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_CIRCULAR);
    easyCam.begin();
    drawParticles();
    easyCam.end();
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
