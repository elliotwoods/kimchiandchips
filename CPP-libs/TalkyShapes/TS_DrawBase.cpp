//
//  TS_DrawBase.cpp
//  TalkyShapes
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_DrawBase.h"

void TS_DrawBase::drawShape(TS_ShapeBase *shape)
{
    if (implementedShapes.count(shape->Type) > 0)
    {
        switch (shape->Type) {
                
            case TS_Type_Quad:
                drawQuad((TS_Quad*) shape);
                break;
                
        }
            
        
    } else        
        TS_Error::passError(TS_ERROR__SHAPE_DRAW_NOT_IMPLEMENTED);
    
}