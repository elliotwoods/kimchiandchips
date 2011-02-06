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

TSVec2f* TS_ShapeBase::getVerticesPointer(int &nVertices)
{
	nVertices = getNVertices();
	
	return vertices;
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
		
		vertices = new TSVec2f[count];
		verticesInitialised = count;
	}
}