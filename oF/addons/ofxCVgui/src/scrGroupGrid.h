/*
 *  scrGroupGrid.h
 *  PC Encode
 *
 *  Created by Elliot Woods on 29/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrGroupBase.h"

class scrGroupGrid : public scrGroupBase
{
public:
	scrGroupGrid(string caption="");
	void		drawContent();
	
	int			findScreen(int x, int y);
	
	void		setGridWidth(int _gridWidth);
	
protected:
	void		doResize();
	int			gridWidth, gridHeight;
};
