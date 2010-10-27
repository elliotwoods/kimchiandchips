/*
 *  scrGroup.h
 *  PC Encode
 *
 *  Created by Elliot Woods on 27/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrBase.h"

class scrGroup : public scrBase
{
	// it's just the same as default. can we use that?
	//	scrGroup(enumShowCursor showCursor, bool hasCursorEvents, string caption);
	
	void						draw(int x, int y, int width, int height);
	
	void						mouseDown(float xX, float xY, int x, int y);
	
	void						arrange();
	
	int							groupType;
	vector<scrBase*>			screens;
	
	
private:
	std::vector<Histogram*>	_vecHistograms;
};