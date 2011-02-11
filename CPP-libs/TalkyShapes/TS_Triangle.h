//
//  TS_Triangle.h
//  TalkyShapes
//
//  Created by Elliot Woods on 07/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_ShapeBase.h"

class TS_Triangle : public TS_ShapeBase
{
public:
	TS_Triangle();
    
    void    init(float x, float y, float scale);
	
	void	serialise(TalkyMessage &msg);
	void	deSerialise(TalkyMessage const &msg);
    
    bool    isHit(Vector2f XY);

};