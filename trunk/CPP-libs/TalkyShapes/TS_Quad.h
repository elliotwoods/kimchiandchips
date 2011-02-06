/*
 *  TS_Quad.h
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TS_ShapeBase.h"

class TS_Quad : public TS_ShapeBase
{
public:
	TS_Quad();
	
	void	serialise(TalkyMessage &msg);
	void	deSerialise(TalkyMessage const &msg);
};