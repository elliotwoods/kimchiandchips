/*
 *  scrPointCloud.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 04/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrPointCloud.h"

scrPointCloud::scrPointCloud(string caption) :
scrBase(cursor_none, false, caption)
{
	pointSize = 2;
	distance = 1;
	glGenBuffersARB(1, &vbo[0]);
	glGenBuffersARB(1, &vbo[1]);
}

void scrPointCloud::drawContent()
{
	ofPushStyle();
	
	//temporarily store the viewport
	int viewport_temp[4];
	glGetIntegerv(GL_VIEWPORT, viewport_temp);
	
	//set the viewport to our screen only
	int boundsx,boundsy,boundswidth,boundsheight;
	getLiveBounds(boundsx, boundsy, boundswidth, boundsheight);
	boundsy = ofGetHeight()-boundsy-boundsheight;
	glViewport(boundsx, boundsy, boundswidth, boundsheight);
	
	//camera
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(50.0f, float(boundswidth)/float(boundsheight), 0.1f, 10.0f);
	gluLookAt(0, 0, -distance, 0, 0, 0, 0, 1, 0);
	
	//modelview
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(spin.x, 1, 0, 0);
	glRotatef(spin.y, 0, 1, 0);
	
	glEnable(GL_DEPTH_TEST);
	glPointSize(pointSize);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, _nPoints * 3 * sizeof(float), _positions, GL_STATIC_DRAW_ARB);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, _nPoints * 3 * sizeof(float), _colours, GL_STATIC_DRAW_ARB);
	glColorPointer(3, GL_FLOAT, 0, 0);
	
	glDrawArrays(GL_POINTS, 0, _nPoints);
	
	glDisableClientState(GL_VERTEX_ARRAY); 
	glDisableClientState(GL_COLOR_ARRAY);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	
	//clear world states
	glDisable(GL_DEPTH_TEST);	
	
	//clear matricies
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	//let's get our full viewport back
	glViewport(viewport_temp[0], viewport_temp[1],
			   viewport_temp[2], viewport_temp[3]);
	
}

void scrPointCloud::mouseDragged(int x, int y, int dx, int dy, int button)
{
	int boundsx, boundsy, boundswidth, boundsheight;
	getLiveBounds(boundsx, boundsy, boundswidth, boundsheight);
	
	if (button==0)
	{
		spin.x += float(dy) / float(boundsheight) * 360.0f;
		spin.y -= float(dx) / float(boundswidth) * 360.0f;
		
		spin.x = ofClamp(spin.x, -90, 90);
	} else {
		distance +=float(dy) / float(boundsheight) * 4.0f;
		distance = ofClamp(distance, 0.1, 10);
	}

}

void scrPointCloud::setWith(float *positions, float *colours, int nPoints)
{
	_positions = positions;
	_colours = colours;
	_nPoints = nPoints;
}
