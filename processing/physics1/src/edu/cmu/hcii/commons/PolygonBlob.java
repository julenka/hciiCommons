package edu.cmu.hcii.commons;

import java.util.ArrayList;
import java.util.Collections;

import org.jbox2d.collision.shapes.ChainShape;
import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.Body;
import org.jbox2d.dynamics.BodyDef;

import processing.core.PApplet;
import processing.core.PVector;
import toxi.geom.Polygon2D;
import toxi.geom.Vec2D;
import blobDetection.Blob;
import blobDetection.BlobDetection;
import blobDetection.EdgeVertex;

//an extended polygon class quite similar to the earlier PolygonBlob class (but extending Toxiclibs' Polygon2D class instead)
//The main difference is that this one is able to create (and destroy) a box2d body from it's own shape
class PolygonBlob extends Polygon2D {
	physics1Sketch parent;
	// to hold the box2d body
	Body body;

	public PolygonBlob(physics1Sketch parent) {
		this.parent = parent;
	}

	// the createPolygon() method is nearly identical to the one presented earlier
	// see the Kinect Flow Example for a more detailed description of this method (again, feel free to improve it)
	void createPolygon() {
		ArrayList<ArrayList<PVector>> contours = new ArrayList<ArrayList<PVector>>();
		int selectedContour = 0;
		int selectedPoint = 0;

		// create contours from blobs
		for (int n=0 ; n<parent.theBlobDetection.getBlobNb(); n++) {
			Blob b = parent.theBlobDetection.getBlob(n);
			if (b != null && b.getEdgeNb() > 100) {
				ArrayList<PVector> contour = new ArrayList<PVector>();
				for (int m=0; m<b.getEdgeNb(); m++) {
					EdgeVertex eA = b.getEdgeVertexA(m);
					EdgeVertex eB = b.getEdgeVertexB(m);
					if (eA != null && eB != null) {
						EdgeVertex fn = b.getEdgeVertexA((m+1) % b.getEdgeNb());
						EdgeVertex fp = b.getEdgeVertexA((parent.max(0, m-1)));
						float dn = parent.dist(eA.x*parent.kinectWidth, eA.y*parent.kinectHeight, fn.x*parent.kinectWidth, fn.y*parent.kinectHeight);
						float dp = parent.dist(eA.x*parent.kinectWidth, eA.y*parent.kinectHeight, fp.x*parent.kinectWidth, fp.y*parent.kinectHeight);
						if (dn > 15 || dp > 15) {
							if (contour.size() > 0) {
								contour.add(new PVector(eB.x*parent.kinectWidth, eB.y*parent.kinectHeight));
								contours.add(contour);
								contour = new ArrayList<PVector>();
							} else {
								contour.add(new PVector(eA.x*parent.kinectWidth, eA.y*parent.kinectHeight));
							}
						} else {
							contour.add(new PVector(eA.x*parent.kinectWidth, eA.y*parent.kinectHeight));
						}
					}
				}
			}
		}

		while (contours.size() > 0) {

			// find next contour
			float distance = 999999999;
			if (getNumPoints() > 0) {
				Vec2D vecLastPoint = vertices.get(getNumPoints()-1);
				PVector lastPoint = new PVector(vecLastPoint.x, vecLastPoint.y);
				for (int i=0; i<contours.size(); i++) {
					ArrayList<PVector> c = contours.get(i);
					PVector fp = c.get(0);
					PVector lp = c.get(c.size()-1);
					if (fp.dist(lastPoint) < distance) { 
						distance = fp.dist(lastPoint); 
						selectedContour = i; 
						selectedPoint = 0;
					}
					if (lp.dist(lastPoint) < distance) { 
						distance = lp.dist(lastPoint); 
						selectedContour = i; 
						selectedPoint = 1;
					}
				}
			} else {
				PVector closestPoint = new PVector(parent.width, parent.height);
				for (int i=0; i<contours.size(); i++) {
					ArrayList<PVector> c = contours.get(i);
					PVector fp = c.get(0);
					PVector lp = c.get(c.size()-1);
					if (fp.y > parent.kinectHeight-5 && fp.x < closestPoint.x) { 
						closestPoint = fp; 
						selectedContour = i; 
						selectedPoint = 0;
					}
					if (lp.y > parent.kinectHeight-5 && lp.x < closestPoint.y) { 
						closestPoint = lp; 
						selectedContour = i; 
						selectedPoint = 1;
					}
				}
			}

			// add contour to polygon
			ArrayList<PVector> contour = contours.get(selectedContour);
			if (selectedPoint > 0) { Collections.reverse(contour); }
			for (PVector p : contour) {
				add(new Vec2D(p.x, p.y));
			}
			contours.remove(selectedContour);
		}
	}

	// creates a shape-deflecting physics chain in the box2d world from this polygon
	void createBody() {
		// for stability the body is always created (and later destroyed)
		BodyDef bd = new BodyDef();
		body = parent.box2d.createBody(bd);
		// if there are more than 0 points (aka a person on screen)...
		if (getNumPoints() > 0) {
			// create a vec2d array of vertices in box2d world coordinates from this polygon
			Vec2[] verts = new Vec2[getNumPoints()];
			for (int i=0; i<getNumPoints(); i++) {
				Vec2D v = vertices.get(i);
				verts[i] = parent.box2d.coordPixelsToWorld(v.x, v.y);
			}
			// create a chain from the array of vertices
			ChainShape chain = new ChainShape();
			chain.createChain(verts, verts.length);
			// create fixture in body from the chain (this makes it actually deflect other shapes)
			body.createFixture(chain, 1);
		}
	}

	// destroy the box2d body (important!)
	void destroyBody() {
		parent.box2d.destroyBody(body);
	}
}
