package edu.cmu.hcii.commons;

import org.jbox2d.collision.shapes.CircleShape;
import org.jbox2d.collision.shapes.PolygonShape;
import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.Body;
import org.jbox2d.dynamics.BodyDef;
import org.jbox2d.dynamics.BodyType;
import org.jbox2d.dynamics.FixtureDef;

import toxi.geom.Circle;
import toxi.geom.Polygon2D;
import toxi.geom.Vec2D;

//usually one would probably make a generic Shape class and subclass different types (circle, polygon), but that
//would mean at least 3 instead of 1 class, so for this tutorial it's a combi-class CustomShape for all types of shapes
//to save some space and keep the code as concise as possible I took a few shortcuts to prevent repeating the same code
class CustomShape {
	physics1Sketch parent;
	// to hold the box2d body
	Body body;
	// to hold the Toxiclibs polygon shape
	Polygon2D toxiPoly;
	// custom color for each shape
	int col;
	// radius (also used to distinguish between circles and polygons in this combi-class
	float r;
	char c;

	CustomShape(physics1Sketch parent, float x, float y, float r) {
		this.parent = parent;
		this.r = r;
		// create a body (polygon or circle based on the r)
		makeBody(x, y);
		// get a random color
		col = parent.getRandomColor();
		c = "hcii".charAt((int)parent.random(4));
	}

	void makeBody(float x, float y) {
		// define a dynamic body positioned at xy in box2d world coordinates,
		// create it and set the initial values for this box2d body's speed and angle
		BodyDef bd = new BodyDef();
		bd.type = BodyType.DYNAMIC;
		bd.position.set(parent.box2d.coordPixelsToWorld(new Vec2(x, y)));
		body = parent.box2d.createBody(bd);
		body.setLinearVelocity(new Vec2(parent.random(-8, 8), parent.random(2, 8)));
		body.setAngularVelocity(parent.random(-5, 5));

		// depending on the r this combi-code creates either a box2d polygon or a circle
		// box2d circle shape of radius r
		CircleShape cs = new CircleShape();
		cs.m_radius = parent.box2d.scalarPixelsToWorld(r);
		// tweak the circle's fixture def a little bit
		FixtureDef fd = new FixtureDef();
		fd.shape = cs;
		fd.density = 1;
		fd.friction = 0.01f;
		fd.restitution = 0.3f;
		// create the fixture from the shape's fixture def (deflect things based on the actual circle shape)
		body.createFixture(fd);
	
	}

	// method to loosely move shapes outside a person's polygon
	// (alternatively you could allow or remove shapes inside a person's polygon)
	void update() {
		// get the screen position from this shape (circle of polygon)
		Vec2 posScreen = parent.box2d.getBodyPixelCoord(body);
		// turn it into a toxiclibs Vec2D
		Vec2D toxiScreen = new Vec2D(posScreen.x, posScreen.y);
		// check if this shape's position is inside the person's polygon
		boolean inBody = parent.poly.containsPoint(toxiScreen);
		// if a shape is inside the person
		if (inBody) {
			// find the closest point on the polygon to the current position
			Vec2D closestPoint = toxiScreen;
			float closestDistance = 9999999;
			for (Vec2D v : parent.poly.vertices) {
				float distance = v.distanceTo(toxiScreen);
				if (distance < closestDistance) {
					closestDistance = distance;
					closestPoint = v;
				}
			}
			// create a box2d position from the closest point on the polygon
			Vec2 contourPos = new Vec2(closestPoint.x, closestPoint.y);
			Vec2 posWorld = parent.box2d.coordPixelsToWorld(contourPos);
			float angle = body.getAngle();
			// set the box2d body's position of this CustomShape to the new position (use the current angle)
			body.setTransform(posWorld, angle);
		}
	}

	// display the customShape
	void display() {
		// get the pixel coordinates of the body
		Vec2 pos = parent.box2d.getBodyPixelCoord(body);
		parent.pushMatrix();
		// translate to the position
		parent.translate(pos.x, pos.y);
		parent.noStroke();
		// use the shape's custom color
		parent.fill(col);
		// depending on the r this combi-code displays either a polygon or a circle
		
		float a = body.getAngle();
		parent.rotate(-a); // minus!
		parent.ellipse(0, 0, r*2, r*2);
		parent.fill(parent.color(255,255,255));
		parent.text(c, 0, 0);
		parent.popMatrix();
	}

	// if the shape moves off-screen, destroy the box2d body (important!)
	// and return true (which will lead to the removal of this CustomShape object)
	boolean done() {
		Vec2 posScreen = parent.box2d.getBodyPixelCoord(body);
		boolean offscreen = posScreen.y > parent.height;
		if (offscreen) {
			parent.box2d.destroyBody(body);
			return true;
		}
		return false;
	}
}
