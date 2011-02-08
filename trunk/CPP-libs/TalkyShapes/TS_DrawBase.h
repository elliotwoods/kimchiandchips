#pragma once
//
//  TS_DrawBase.h
//  iPadExample
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_ShapeBase.h"
#include "TS_Includes.h"
#include "TS_Shapes.h"

#include <set>

class TS_DrawBase
{
    public:
        void    drawShape(TS_ShapeBase *shape);
    
    protected:
//        virtual void    drawPoint(TS_Point *shape) = 0;
//        virtual void    drawLine(TS_Line *shape) = 0;
        virtual void    drawQuad(TS_Quad *shape) = 0;
    
        set<TS_ShapeType>   implementedShapes;
};