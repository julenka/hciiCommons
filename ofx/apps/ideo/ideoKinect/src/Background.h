//
//  Background.h
//  fallingSand
//
//  Created by Julia Schwarz on 10/6/13.
//
//

#ifndef __fallingSand__Background__
#define __fallingSand__Background__

#include <iostream>
#include "constants.h"
#include "ofxKinect.h"

class Background {
public:
    Background();
    ~Background();
    
    void update(ofxKinect &kinect);
    bool isBackground(ofxKinect &kinect, int x, int y);
    void setup();
    void drawMeanBg(int x, int y, int w, int h);
    void drawStdevBg(int x, int y, int w, int h);
    void drawSmoothedBg(int x, int y, int w, int h);

    unsigned char* getForegroundPixels() { return foregroundPixels; };
    
    int getBgFrameCount() { return bgFrameCount; };
    void setSmoothFactor(float v) {smoothFactor = v;};
private:
    float smoothFactor;
    ofTexture bgTexture;
    ofPixels bgPixels;
    ofTexture stdTexture;
    ofPixels stdPixels;
    ofColor c;
    bool dirtyPixels;
    float backgroundSqSum[DEPTH_MAP_SIZE];
    float backgroundSum[DEPTH_MAP_SIZE];
    float backgroundMean[DEPTH_MAP_SIZE];
    float backgroundStdev[DEPTH_MAP_SIZE];
    float backgroundSmoothed[DEPTH_MAP_SIZE];
    int backgroundCount[DEPTH_MAP_SIZE];
    unsigned char foregroundPixels[DEPTH_MAP_SIZE];
    int bgFrameCount = 0;

};

#endif /* defined(__fallingSand__Background__) */
