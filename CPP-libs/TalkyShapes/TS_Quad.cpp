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
	
	Type = TS_SHAPE_TYPE_QUAD;
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
	if (msg.Type < TS_MSG_RANGE_OBJECT_SINGLE_MIN || msg.Type > TS_MSG_RANGE_OBJECT_SINGLE_MAX)
		throw("TS_ShapeBase: Incoming data message doesn't refer to single object");
	
	int PayloadLength;
	char* Payload = msg.getPayload(PayloadLength);
	
	//if we have a fixed length for vertices data
	//then we can do some checks before reading message further (*)
	if (nVerticesFixed)
	{
		if (PayloadLength != getNBytesTotal())
			throw("TS_ShapeBase::deSerialise : Can't deserialise this object message to this (fixed vertices) object type. Message length doesn't match");
		
		if (Payload[getNBytesTotal()-1] != TS_PAYLOAD_TERMINATOR)
			throw("TS_ShapeBase::deSerialise : Terminator doesn't match, perhaps data is corrupt?");
			
	}
	
	unsigned short msgType = *(unsigned short*) (Payload+4);
	unsigned char msgVerticesX = *(unsigned short*) (Payload+6);
	unsigned char msgVerticesY = *(unsigned short*) (Payload+7);
	
	if (msgType != Type)
		throw("TS_ShapeBase::deSerialise : Can't deserialise this object message. Message type doesn't match");
	
	if (nVerticesFixed)
		if (msgVerticesX != nVerticesX || msgVerticesY != nVerticesY)
			throw("TS_ShapeBase::deSerialise : Can't deserialise this object message. nVerticesX/Y deosn't match");
	
	ID = *(unsigned long*) Payload;
	
	if (!nVerticesFixed)
	{
		nVerticesX = 	*(unsigned char *)		(Payload+6);
		nVerticesY = 	*(unsigned char *)		(Payload+7);
		
		//since we now know the vertices count
		//we can perform the checks from (*) here
		
		if (PayloadLength != getNBytesTotal())
			throw("TS_ShapeBase::deSerialise : Can't deserialise this object message to this (non fixed vertices) object type. Message length doesn't match");
		
		if (Payload[getNBytesTotal()-1] != TS_PAYLOAD_TERMINATOR)
			throw("TS_ShapeBase::deSerialise : Terminator doesn't match, perhaps data is corrupt?");
	}
	
	//setup our array
	initialiseVertices();
	
	//copy vertices payload into our array
	memcpy(vertices, Payload + 8, getNBytesVertices());
	
	return;
}