//
//  Background.cpp
//  fallingSand
//
//  Created by Julia Schwarz on 10/6/13.
//
//

#include "Background.h"


Background::Background() : dirtyPixels(true), smoothFactor(0.9) {}

Background::~Background() {}

void Background::setup() {
    for(int i = 0; i < DEPTH_MAP_SIZE; i++) {
        backgroundMean[i] = 0;
        backgroundStdev [i] = 0;
        backgroundSum[i] = 0;
        backgroundSqSum[i] = 0;
        backgroundCount[i] = 0;
        backgroundSmoothed[i] = 0;
    }
    bgPixels.allocate(DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT, OF_PIXELS_RGB);
    stdPixels.allocate(DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT, OF_PIXELS_RGB);
}


void Background::update(ofxKinect &kinect) {
    bgFrameCount++;
    for(int y = 0; y < DEPTH_MAP_HEIGHT; y++) {
        for(int x = 0; x < DEPTH_MAP_WIDTH; x++) {
            int i = y * DEPTH_MAP_WIDTH + x;
            float depth = kinect.getDistanceAt(x, y);
            if(depth <= 0 ) continue;
            backgroundSmoothed[i] = backgroundSmoothed[i] * smoothFactor + depth * (1 - smoothFactor);
        }
    }
    dirtyPixels = true;
}

bool Background::isBackground(ofxKinect &kinect, int x, int y) {
    int i = y * DEPTH_MAP_WIDTH + x;
    float depth = kinect.getDistanceAt(x, y);
    if(depth < 500) return true;
    if(depth - backgroundSmoothed[i] > -300) return true;
    return false;
}

void Background::drawMeanBg(int x, int y, int w, int h) {
    if(dirtyPixels) {
        for(int y = 0; y < DEPTH_MAP_HEIGHT; y++) {
            for(int x = 0; x < DEPTH_MAP_WIDTH; x++) {
                int i = y * DEPTH_MAP_WIDTH + x;
                float depth = backgroundMean[i];
                int hue = ofMap(depth, 500, 30000, 0, 255, true);
                c = ofColor::fromHsb(hue, 255,255 );
                bgPixels[i * 3]= c.r;
                bgPixels[i * 3 + 1]= c.g;
                bgPixels[i * 3 + 2]= c.b;
            }
        }
    }
    bgTexture.loadData(bgPixels);
    bgTexture.draw(x,y,w,h);
}

void Background::drawSmoothedBg(int x, int y, int w, int h) {
    if(dirtyPixels) {
        for(int y = 0; y < DEPTH_MAP_HEIGHT; y++) {
            for(int x = 0; x < DEPTH_MAP_WIDTH; x++) {
                int i = y * DEPTH_MAP_WIDTH + x;
                float depth = backgroundSmoothed[i];
                int hue = ofMap(depth, 500, 30000, 0, 255, true);
                c = ofColor::fromHsb(hue, 255,255 );
                bgPixels[i * 3]= c.r;
                bgPixels[i * 3 + 1]= c.g;
                bgPixels[i * 3 + 2]= c.b;
            }
        }
    }
    bgTexture.loadData(bgPixels);
    bgTexture.draw(x,y,w,h);
}


void Background::drawStdevBg(int x, int y, int w, int h) {
    if(dirtyPixels) {
        for(int y = 0; y < DEPTH_MAP_HEIGHT; y++) {
            for(int x = 0; x < DEPTH_MAP_WIDTH; x++) {
                int i = y * DEPTH_MAP_WIDTH + x;
                float depth = backgroundStdev[i];
                int hue = ofMap(depth, 500, 100, 0, 255, true);
                c = ofColor::fromHsb(hue, 255,255 );
                stdPixels[i * 3]= c.r;
                stdPixels[i * 3 + 1]= c.g;
                stdPixels[i * 3 + 2]= c.b;
            }
        }
    }
    stdTexture.loadData(stdPixels);
    stdTexture.draw(x,y,w,h);
}
