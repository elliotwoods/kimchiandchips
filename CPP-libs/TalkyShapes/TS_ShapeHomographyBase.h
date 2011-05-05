//
//  TS_ShapeHomographyBase.h
//  TalkyShapes
//
//  Created by Elliot Woods on 07/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_ShapeBase.h"

class TS_ShapeHomographyBase : public TS_ShapeBase
{
    public:
        TS_ShapeHomographyBase();
    
        Matrix4f    getHomography();
        Matrix4f    getInverseHomography();
    
        bool        isHit(Vector2f XY);
        Vector2f    applyTo(Vector2f XY);
    
        int         homographyPoints[4];
    
    protected:
        static Vector2f     VerticesSource[4];
};