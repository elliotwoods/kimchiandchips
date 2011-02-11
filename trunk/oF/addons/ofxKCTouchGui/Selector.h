#pragma once
/*
 *  Selector.h
 *  Quad Mapper app
 *
 *  Created by Elliot Woods on 28/09/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "GUI_element.h"

class Selector : public GUI_element
{
	public:
		Selector(float inx, float iny, float inw, float inh,
			  int nItems,
			  string caption);
    
		void		draw();
		void		update();
		
		void		touchMoved(Touch *_touch);
		void        touchUp(Touch *_touch);
		void        touchDown(Touch *_touch);
	
		int			selection;
	
		string			_caption;
		vector<string>	_items;
		int				_nItems;
	
};