#include "testApp.h"

#define DEPTH_MAP_WIDTH 640
#define DEPTH_MAP_HEIGHT 480
#define BORN_TIMEOUT_MS 600
#define FALL_TIMEOUT_MS 4000
#define GRAVITY 4
#define RANDOM_THRESH 100
#define BACKGROUND_SMOOTHING 0.99
#define BACKGROUND_DISTANCE_THRESH 1000
#define FLOOR_THRESH 1000
#define NUM_BG_FRAMES 100
#define SCENE_RADIUS 3000
#define CAMERA_PERIOD_MS 10000

void testApp::makeParticleAt(const ofVec3f &pt) {
    if(inactiveParticles.size() == 0) {
        return;
    }
    int idx = inactiveParticles.front();
    Particle *p = &particles[idx];
    p->x = pt.x + ofRandom( -100, 100);
    p->y = pt.y + ofRandom( -100, 100);
    p->z = pt.z + ofRandom( -100, 100);
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
        float pct;
        int gray;
        switch(p->state) {
            case INACTIVE:
                break;
            case BORN:
                mesh.addColor(ofColor(255,255,255));
                mesh.addVertex(ofVec3f(p->x, p->y, p->z));
                break;
            case FALLING:
                pct = ((ofGetSystemTime() - p->bornTime) / (float)(FALL_TIMEOUT_MS - BORN_TIMEOUT_MS));
                if(pct > 1) pct = 1;
                gray = (int)(255 * (1 - pct));
                mesh.addColor(ofColor(gray));
                mesh.addVertex(ofVec3f(p->x, p->y, p->z));
                break;
            default:
                break;
        }
    }
	glPointSize(2);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(-1, -1, 1);
	ofTranslate(0, 0, -SCENE_RADIUS); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}

void testApp::updateParticles() {
    center = ofVec3f(0,0,0);
    int particleCount = 0;
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
                    if(p->y < FLOOR_THRESH) {
                        p->vx += p ->ax;
                        p->vy += p->ay;
                        p->vz += p->az;
                        p->x += p->vx;
                        p->y += p->vy;
                        p->z += p->vz;
                    }
                }
                break;
            default:
                break;
                
        }
        
        if(p->state != INACTIVE) {
            particleCount++;
            center.x += p->x;
            center.y += p->y;
            center.z += p->z;
        }
    }
    center /= particleCount;
}

void testApp::updateBackground() {
    bgFrameCount++;
    for(int y = 0; y < DEPTH_MAP_HEIGHT; y++) {
        for(int x = 0; x < DEPTH_MAP_WIDTH; x++) {
            int i = y * DEPTH_MAP_WIDTH + x;
            float depth = kinect.getDistanceAt(x, y);
            if(depth <= 0 ) continue;
            backgroundCount[i]++;
            backgroundMean[i] += depth;
            // e[(x - xbar)^2)] = e[x^2] - e[x]^2
            backgroundStdev[i] += (depth * depth);
        }
    }
    if(bgFrameCount == NUM_BG_FRAMES) {
        for(int i = 0; i < DEPTH_MAP_SIZE; i++) {
            if(backgroundCount[i] == 0) continue;
            backgroundMean[i] /= backgroundCount[i];
            backgroundStdev[i] /= backgroundCount[i];
            backgroundStdev[i] -= backgroundMean[i] * backgroundMean[i];
            backgroundStdev[i] = sqrt(backgroundStdev[i]);
        }
    }
}

void testApp::drawDebugText() {
    ofPushStyle();
    char buf[100];
    sprintf(buf, "inactive size: %ld\nfps: %f\nbg: %d/%d", inactiveParticles.size(), ofGetFrameRate(), bgFrameCount, NUM_BG_FRAMES);
    ofSetColor(255, 0, 255);
    ofDrawBitmapString(buf, ofPoint(10,10));
    ofPopStyle();
}

void testApp::drawDebugInfo() {
    // x is red
    ofPushStyle();
    ofSetColor(255,0,0);
    ofPushMatrix();
    ofRotate(-90, 0, 0, 1);
    ofDrawCylinder(0, 0, 10, 50);
    ofPopMatrix();

    // y is green
    ofSetColor(0,255,0);
    ofPushMatrix();
    ofDrawCylinder(0, 0, 10, 50);
    ofPopMatrix();

    // z is blue
    ofSetColor(0,0,255);
    ofPushMatrix();
    ofRotate(90, 1, 0, 0);
    ofDrawCylinder(0, 0, 10, 50);
    ofPopMatrix();

    
    ofPopStyle();
}

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
    for(int i = 0; i < DEPTH_MAP_SIZE; i++) {
        backgroundMean[i] = 0;
        backgroundStdev [i] = 0;
    }
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
    // update camera
    float rad = (float)(ofGetElapsedTimeMillis() % CAMERA_PERIOD_MS) / CAMERA_PERIOD_MS * 2 * PI;
    camera.setPosition(SCENE_RADIUS * cos(rad), 0, SCENE_RADIUS * sin(rad));
    camera.lookAt(ofVec3f(0,0,0));
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
        if(bgFrameCount < NUM_BG_FRAMES) {
            updateBackground();
            return;
        }
        for(int y = 0; y < DEPTH_MAP_HEIGHT; y++) {
            for(int x = 0; x < DEPTH_MAP_WIDTH; x++) {
                int i = y * DEPTH_MAP_WIDTH + x;
                ofVec3f p = kinect.getWorldCoordinateAt(x, y);
                float depth = kinect.getDistanceAt(x, y);
                if(depth < 500) continue;
                if(abs(depth - backgroundMean[i]) < 100) continue;
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
    camera.begin();
    drawParticles();
    drawDebugInfo();
    camera.end();
    kinect.drawDepth(0, 0, 160, 120);
    drawDebugText();
}

//--------------------------------------------------------------
void testApp::exit() {
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
