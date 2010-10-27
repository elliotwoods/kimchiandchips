/*
 *  scrGroup.h
 *  PC Encode
 *
 *  Created by Elliot Woods on 27/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrBase.h"

#define SCRGROUP_TYPE_GRID 0
#define SCRGROUP_TYPE_SWAP 1
#define SCRGROUP_TYPE_TABS 2


class scrGroup : public scrBase
{
public:
	scrGroup();
	void				mouseOver(int x, int y) { };
	void				mouseDown(int x, int y) { };
	void				mouseReleased(int x, int y) { };
	
	void				findScreen(int &x, int &y, int &iScreen, float &xX, float &xY);
	
	int					groupType;
	int					gridWidth;
	
	vector<scrBase*>	screens;
	
	
private:
	void				drawContent();

	void				doResize();
	void				arrangeGrid();
	
	int					gridHeight;
	int					iScreenMaximised;

};