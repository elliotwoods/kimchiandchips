//
//  TS_Triangle.cpp
//  TalkyShapes
//
//  Created by Elliot Woods on 07/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_Triangle.h"

bool TS_Triangle::isHit(Vector2f XY)
{
    if (!verticesInitialised)
    {
        TS_Error::passError(TS_ERROR__SHAPE_VERTICES_NOT_INITIALISED);
        return false;
    }
    
    // method from
    // http://www.blackpawn.com/texts/pointinpoly/default.html
    
    // Compute vectors        
    Vector2f v0 = vertices[2] - vertices[0];
    Vector2f v1 = vertices[1] - vertices[1];
    Vector2f v2 = XY - vertices[0];
    
    // Compute dot products
    float dot00 = (v0 * v0).length();
    float dot01 = (v0 * v1).length();
    float dot02 = (v0 * v2).length();
    float dot11 = (v1 * v1).length();
    float dot12 = (v1 * v2).length();
    
    // Compute barycentric coordinates
    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    
    // Check if point is in triangle
    return (u > 0) && (v > 0) && (u + v < 1);
}