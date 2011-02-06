#pragma  once
/*
 *  TS_ShapeBase.h
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TS_Types.h"
#include "TS_constants.h"

#include <vector>
#include "TalkyMessage.h"

using namespace std;

class TS_ShapeBase
{
public:
	TS_ShapeBase();
	~TS_ShapeBase();
	
	int		getNVertices();
	int		getNVerticesX();
	int		getNVerticesY();
	
	TSVec2f*	getVerticesPointer(int &nVertices);
	
	virtual void	serialise(TalkyMessage &msg);
	virtual void	deSerialise(TalkyMessage const &msg);
    
	unsigned long			ID;
	unsigned short			Type;
	vector<unsigned short>	Tags;
	
	
protected:
	int				getNBytesBase();
	int				getNBytesTotal();
	virtual int		getNBytesVertices();
	
	void			initialiseVertices(int count = -1); //-1 will trigger getNVertices()
	
	unsigned char	nVerticesX;
	unsigned char	nVerticesY;
	bool			nVerticesFixed;
	
	TSVec2f			*vertices;
	int				verticesInitialised;
};
