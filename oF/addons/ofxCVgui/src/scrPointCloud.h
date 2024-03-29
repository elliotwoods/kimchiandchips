#pragma once
/*
 *  scrPointCloud.h
 *  PC Encode
 *
 *  Created by Elliot Woods on 04/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofMain.h"

#include "scrTexture.h"

class scrPointCloud : public scrBase {
	
public:
	scrPointCloud(string caption);
	~scrPointCloud();
	
	void			mouseDragged(int x, int y, int dx, int dy, int button);
	int				pointSize;

    void            clear();
	void			setWith(float *positions, float *colours, int nPoints);
	void			keyPressed(int key);
	
	static ofPoint	spin;
	static ofPoint	translate;
	static float	distance;
    
protected:
	virtual void	drawContent();
    
    void            begin();
    void            drawPoints();
    void            end();
	
	void			* _onMouseOver(float x, float y);

    bool            _isSet;
	float			*_positions, *_colours;
	int				_nPoints;
	GLuint			vbo[2];	
	
    int             _viewport_temp[4];
};