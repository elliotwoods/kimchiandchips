/*
 *  TS_Quad.cpp
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TS_Quad.h"

TS_Quad::TS_Quad()
{
	nVerticesX = 2;
	nVerticesY = 2;
	nVerticesFixed = true;
	
	Type = TS_Type_Quad;
    
    initialiseVertices();
}

void TS_Quad::init(TS_ShapePalette *Palette, float x, float y, float scale)
{
    this->Palette = Palette;
    
    scale *= 0.5f;
    //TL
    vertices[0].x = x - scale;
    vertices[0].y = y - scale;
    
    //TR
    vertices[1].x = x + scale;
    vertices[1].y = y - scale;
    
    //BR
    vertices[2].x = x + scale;
    vertices[2].y = y + scale;
    
    //BL
    vertices[3].x = x - scale;
    vertices[3].y = y + scale;   
}


void TS_Quad::serialise(TalkyMessage &msg)
{
	
	int totalBytes = getNBytesTotal();
	char* Payload = new char[totalBytes];
	
	*(unsigned long *)		Payload			= ID;
	*(unsigned short *)		(Payload+4)		= Type;
	*(unsigned char *)		(Payload+6)		= nVerticesX;
	*(unsigned char *)		(Payload+7)		= nVerticesY;
	
	memcpy(Payload+8, vertices, 8 * getNVertices());
	
	*(Payload + totalBytes - 1)				= TS_PAYLOAD_TERMINATOR;
	
	
	msg.setPayload(Payload, totalBytes);
	
	
	delete[] Payload;
	
	return;
}

void TS_Quad::deSerialise(TalkyMessage const &msg)
{
	if (msg.ContentsType < TS_MSG_RANGE_OBJECT_SINGLE_MIN || msg.ContentsType > TS_MSG_RANGE_OBJECT_SINGLE_MAX)
			TS_Error::passError(TS_ERROR__DESERIALISE_MISMATCH_PLURALITY_SINGLE);
	
	int PayloadLength;
	char* Payload = msg.getPayload(PayloadLength);
	
	//if we have a fixed length for vertices data
	//then we can do some checks before reading message further (*)
	if (nVerticesFixed)
	{
		if (PayloadLength != getNBytesTotal())
			TS_Error::passError(TS_ERROR__DESERIALISE_MISMATCH_MESSAGE_LENGTH);
		
		if (Payload[getNBytesTotal()-1] != TS_PAYLOAD_TERMINATOR)
            TS_Error::passError(TS_ERROR__DESERIALISE_MISMATCH_TERMINATOR);
			
	}
	
	unsigned short msgType = *(unsigned short*) (Payload+4);
	unsigned char msgVerticesX = *(unsigned short*) (Payload+6);
	unsigned char msgVerticesY = *(unsigned short*) (Payload+7);
	
	if (msgType != Type)
		TS_Error::passError(TS_ERROR__DESERIALISE_MISMATCH_SHAPE_TYPE);
	
	if (nVerticesFixed)
		if (msgVerticesX != nVerticesX || msgVerticesY != nVerticesY)
			TS_Error::passError(TS_ERROR__DESERIALISE_MISMATCH_NVERTICES);
	
	ID = *(unsigned long*) Payload;
	
	if (!nVerticesFixed)
	{
		nVerticesX = 	*(unsigned char *)		(Payload+6);
		nVerticesY = 	*(unsigned char *)		(Payload+7);
		
		//since we now know the vertices count
		//we can perform the checks from (*) here
		
		if (PayloadLength != getNBytesTotal())
			TS_Error::passError(TS_ERROR__DESERIALISE_MISMATCH_MESSAGE_LENGTH);
		
		if (Payload[getNBytesTotal()-1] != TS_PAYLOAD_TERMINATOR)
            TS_Error::passError(TS_ERROR__DESERIALISE_MISMATCH_TERMINATOR);
	}
	
	//setup our array
	initialiseVertices();
	
	//copy vertices payload into our array
	memcpy(vertices, Payload + 8, getNBytesVertices());
	
	return;
}
