#pragma once
/*
 *  TS_Quad.h
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TS_ShapeHomographyBase.h"
#include "Homography.h"

class TS_Quad : public TS_ShapeHomographyBase
{
public:
	TS_Quad();
    
    void    init(TS_ShapePalette *
                 Palette, float x, float y, float scale);
	
	void	serialise(TalkyMessage &msg);
	void	deSerialise(TalkyMessage const &msg);
};