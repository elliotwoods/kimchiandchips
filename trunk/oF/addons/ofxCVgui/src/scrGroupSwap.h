/*
 *  scrGroupSwap.h
 *  PC Encode
 *
 *  Created by Elliot Woods on 29/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrGroupBase.h"

class scrGroupSwap : public scrGroupBase
{
public:
	scrGroupSwap() { iSelection = 0; };
	int			iSelection;
};
