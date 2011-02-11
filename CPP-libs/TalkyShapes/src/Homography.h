#pragma once
//
//  Homography.h
//  TalkyShapes
//
//  Created by Elliot Woods on 07/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

/*
*  Based entirely on arturo castro's homography implementation
*  Created 08/01/2010, arturo castro
*
*	http://www.openframeworks.cc/forum/viewtopic.php?f=9&t=3121
*/

#include "vmath.h"

class HomographyHelper
{
public:
    static Matrix4f     homography(Vector2f source[4], Vector2f target[4]);
    
protected:
	static void			gaussian_elimination(float *input, int n);
	
	static void			findHomography(float src[4][2], float dst[4][2], float homography[16]);

};