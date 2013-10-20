package edu.cmu.hcii.commons;

import processing.core.PApplet;
import processing.core.PImage;
import SimpleOpenNI.SimpleOpenNI;
//import library
//declare SimpleOpenNI object


public class physics1Sketch extends PApplet {
	// declare SimpleOpenNI object
	SimpleOpenNI context;

	// PImage to hold incoming imagery
	PImage cam;

	public void setup() {
		System.loadLibrary("SimpleOpenNI");

		// same as Kinect dimensions
		size(640, 480);
		// initialize SimpleOpenNI object
		context = new SimpleOpenNI(this);
		context.enableDepth();
		if (!context.enableUser()) {
			// if context.enableScene() returns false
			// then the Kinect is not working correctly
			// make sure the green light is blinking
			println("Kinect not connected!");
			exit();
		} 
	}

	public void draw() {
		// update the SimpleOpenNI object
		context.update();
		// put the image into a PImage
		cam = context.userImage().get();
		// display the image
		image(cam, 0, 0);
	}

}
