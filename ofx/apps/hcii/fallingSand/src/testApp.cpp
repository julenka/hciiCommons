#include "testApp.h"


void testApp::makeParticleAt(const ofVec3f &pt, ofColor c) {
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
    p->baseColor = c;
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
    // TODO: clean these up.
	// the projected points are 'upside down' and 'backwards'
	ofTranslate(0, 0, 0); // center the points a bit
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

void testApp::drawFloor() {
    ofPushStyle();
    ofPushMatrix();
    int sideLengthMM = 5000;
    int minX = -sideLengthMM / 2;
    int maxX = sideLengthMM / 2;
    int minZ = 500;
    int maxZ = minZ + sideLengthMM;
    int gridSpacingMM = 100;
    // grid of size 1m

    // z goes from 1000mm to 2000mm
    
    for(int z = minZ; z <= maxZ; z+= gridSpacingMM) {
        ofSetColor(ofColor::fromHsb(ofMap(z, minZ, maxZ, 0, 180, true), 255, 255));
        ofLine(minX, FLOOR_THRESH, z, maxX, FLOOR_THRESH, z);
    }
    for(int x = minX; x <= maxX; x += gridSpacingMM) {
        ofSetColor(ofColor::fromHsb(ofMap(x, minX, maxX, 0, 180, true), 255, 255));
        ofLine(x, FLOOR_THRESH, minZ, x, FLOOR_THRESH, maxZ);
    }
    // x goes from -500mm to 500mm
    ofPopMatrix();
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

    easyCam.setPosition(0,0,0);
    easyCam.lookAt(ofVec3f(0,0,1), ofVec3f(0,1,0));

	ofSetFrameRate(60);
    cameraDirection = 0;
}

//--------------------------------------------------------------
void testApp::update() {
	kinect.update();
    int particleIndex;
    updateParticles();
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
        if(background.getBgFrameCount() < NUM_BG_FRAMES) {
            background.update(kinect);
            return;
        }
        
        particleIndex = 0;
//        background.update(kinect);
        for(int y = 0; y < DEPTH_MAP_HEIGHT; y++) {
            for(int x = 0; x < DEPTH_MAP_WIDTH; x++) {
                if (particleIndex >= PARTICLE_COUNT) continue;
                int i = y * DEPTH_MAP_WIDTH + x;
                ofVec3f p = kinect.getWorldCoordinateAt(x, y);
                // camera gives us things upside down for some reason.
                p.y *= -1;
                if(background.isBackground(kinect, x, y))continue;
                if(ofRandom(0, RANDOM_THRESH) <= 1) {
                    makeParticleAt(p, ofColor::white);
                }
            }
        }
	}
    ofVec3f cameraPos = easyCam.getPosition();
    int speed = 20;
    switch (cameraDirection) {
        case OF_KEY_LEFT:
            cameraPos.x += speed;
            break;
        case OF_KEY_RIGHT:
            cameraPos.x -= speed;
            break;
        case OF_KEY_UP:
            cameraPos.y+=speed;
        case OF_KEY_DOWN:
            cameraPos.y-=speed;
        default:
            break;
    }
    easyCam.setPosition(cameraPos);

}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackgroundGradient(ofColor(10,10,10), ofColor::black, OF_GRADIENT_CIRCULAR);
    easyCam.begin();
    drawFloor();
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
        case OF_KEY_LEFT:
        case OF_KEY_RIGHT:
        case OF_KEY_UP:
        case OF_KEY_DOWN:
            cameraDirection = key;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased (int key) {
	switch (key) {
        case OF_KEY_LEFT:
        case OF_KEY_RIGHT:
        case OF_KEY_UP:
        case OF_KEY_DOWN:
            cameraDirection = 0;
            break;
        default:
            break;
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
