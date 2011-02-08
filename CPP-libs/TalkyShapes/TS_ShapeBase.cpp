/*
 *  TS_ShapeBase.cpp
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TS_ShapeBase.h"

TS_ShapeBase::TS_ShapeBase() :
verticesInitialised(0)
{
	
}

TS_ShapeBase::~TS_ShapeBase()
{
	if (verticesInitialised > 0)
		delete[] vertices;
}

void TS_ShapeBase::init(Vector2f XY, float scale)
{
    init(XY.x, XY.y, scale);
}

int TS_ShapeBase::getNVertices()
{
	return int(nVerticesX) * int(nVerticesY);
}

int TS_ShapeBase::getNVerticesX()
{
	return (int) nVerticesX;
}

int TS_ShapeBase::getNVerticesY()
{
	return (int) nVerticesY;
}

Vector2f* TS_ShapeBase::getVerticesPointer()
{
    return vertices;
}

Vector2f* TS_ShapeBase::getVerticesPointer(int &nVertices)
{
	nVertices = getNVertices();
	
	return getVerticesPointer();
}

int TS_ShapeBase::getClosestVertex(Vector2f const pipet, float radius)
{
    if (!verticesInitialised)
    {
        TS_Error::passError(TS_ERROR__SHAPE_VERTICES_NOT_INITIALISED);
        return -1;
    }
    
    float closestDistanceSq = radius * radius;
    float distance;
    Vector2f dx;
    
    int iClosest = -1;
    int nVertices = getNVertices();
    
    for (int i=0; i < nVertices; i++)
    {
        dx = pipet - vertices[i];
        distance = dx.lengthSq();
        
        if (distance < closestDistanceSq) {
            iClosest = i;
            closestDistanceSq = distance;
        }
    }
    
    return iClosest;
}

Vector2f TS_ShapeBase::getVertexXY(int iVertex)
{
    if (!verticesInitialised)
    {
        TS_Error::passError(TS_ERROR__SHAPE_VERTICES_NOT_INITIALISED);
        return Vector2f(0,0);
    }
    
    if (iVertex < 0 || iVertex >= getNVertices())
    {
        TS_Error::passError(TS_ERROR__SHAPE_VERTEX_INDEX_OUT_RANGE);
    }
    
    return vertices[iVertex];
    
}

void TS_ShapeBase::moveVertex(int iVertex, Vector2f const &dXY)
{
    if (!verticesInitialised)
    {
        TS_Error::passError(TS_ERROR__SHAPE_VERTICES_NOT_INITIALISED);
        return;
    }
    
    if (iVertex < 0 || iVertex >= getNVertices())
    {
        TS_Error::passError(TS_ERROR__SHAPE_VERTEX_INDEX_OUT_RANGE);
    }
    
    vertices[iVertex] += dXY;
}

void TS_ShapeBase::moveShape(Vector2f const &dXY)
{
    if (!verticesInitialised)
    {
        TS_Error::passError(TS_ERROR__SHAPE_VERTICES_NOT_INITIALISED);
        return;
    }
    
    int nVertices = getNVertices();
    for (int i=0; i<nVertices; i++)
    {
        vertices[i] += dXY;
    }    
}

int TS_ShapeBase::getNBytesBase()
{
	return	4	//ID
		+	2	//Type
		+	1	//nVerticesX
		+	1	//nVerticesY
				//Vertices
		+	1;	//End Byte
}

int TS_ShapeBase::getNBytesTotal()
{
	return getNBytesBase() + getNBytesVertices();
}

int TS_ShapeBase::getNBytesVertices()
{
	//2 floats per vertex
	return getNVertices() * 2 * 4;
}

void TS_ShapeBase::initialiseVertices(int count)
{
	if (count == -1)
		count = getNVertices();
	
	if (verticesInitialised != count)
	{
		if (verticesInitialised > 0)
			delete[] vertices;
		
		vertices = new Vector2f[count];
		verticesInitialised = count;
	}
}