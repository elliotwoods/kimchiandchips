//
//  TS_ShapeHomographyBase.cpp
//  TalkyShapes
//
//  Created by Elliot Woods on 07/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_ShapeHomographyBase.h"

Vector2f TS_ShapeHomographyBase::VerticesSource[4] = {
    Vector2f(-1, -1),
    Vector2f(+1, -1),
    Vector2f(+1, +1),
    Vector2f(-1, +1)
};


TS_ShapeHomographyBase::TS_ShapeHomographyBase()
{
    homographyPoints[0] = 0;
    homographyPoints[1] = 1;
    homographyPoints[2] = 2;
    homographyPoints[3] = 3;
}

Matrix4f TS_ShapeHomographyBase::getHomography()
{
    Vector2f target[4];
    
    for (int i=0; i<4; i++)
        target[i] = vertices[homographyPoints[i]];
    
    return HomographyHelper::homography(VerticesSource, target);
}

Matrix4f TS_ShapeHomographyBase::getInverseHomography()
{
    Vector2f target[4];
    
    for (int i=0; i<4; i++)
        target[i] = vertices[homographyPoints[i]];
    
    return HomographyHelper::homography(target, VerticesSource);
}

bool TS_ShapeHomographyBase::isHit(Vector2f XY)
{
    Vector2f pointInShape = getInverseHomography().applyTo(XY);
    
    TS_Error::debugXY = pointInShape;
    
    return (pointInShape.x <= 1 && pointInShape.x >= -1 && pointInShape.y <= 1 && pointInShape.y >= -1);
}

Vector2f TS_ShapeHomographyBase::applyTo(Vector2f XY)
{
    return getHomography().applyTo(XY);
}