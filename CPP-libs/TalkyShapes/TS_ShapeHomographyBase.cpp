//
//  TS_ShapeHomographyBase.cpp
//  iPadExample
//
//  Created by Elliot Woods on 07/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_ShapeHomographyBase.h"

Vector2f TS_ShapeHomographyBase::VerticesTarget[4] = { Vector2f(-1, -1), Vector2f(+1, -1), Vector2f(+1, +1), Vector2f(-1, +1)};


TS_ShapeHomographyBase::TS_ShapeHomographyBase()
{
    homographyPoints[0] = 0;
    homographyPoints[1] = 1;
    homographyPoints[2] = 2;
    homographyPoints[3] = 3;
}

Matrix4f TS_ShapeHomographyBase::getHomography()
{
    Vector2f source[4];
    
    for (int i=0; i<4; i++)
        source[i] = vertices[homographyPoints[i]];
    
    return HomographyHelper::homography(source, VerticesTarget);
}