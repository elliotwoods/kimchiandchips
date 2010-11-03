/*
 *  scrWidgets.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 02/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrWidgets.h"

#define FOREACH_WIDGET for (int iWidget=0; iWidget<_widgets.size(); iWidget++)

scrWidgets::scrWidgets(string _caption)
: scrBase(cursor_none, true, _caption)
{
	_sumHeights=WIDGET_PADDING;
}

void scrWidgets::mouseDown(int x, int y)
{
	int currentx, currenty, currentw, currenth;
	getLiveBounds(currentx, currenty, currentw, currenth);
	
	if (y<0)
		return;
	
	int yOffset=y-currenty-WIDGET_PADDING;
	
	FOREACH_WIDGET
	{
		if  (yOffset <= _widgets[iWidget]->height)
		{
			_widgets[iWidget]->mouseDown(x,y);
			break;
		}
		
		yOffset -= _widgets[iWidget]->height + WIDGET_PADDING*2;
	}
		
}

void scrWidgets::push(wdgBase *widget)
{
	int x, y, w, h;
	getLiveBounds(x, y, w, h);
	
	widget->setBounds(x+WIDGET_PADDING, y+_sumHeights+WIDGET_PADDING,
					  w - 2*WIDGET_PADDING);
	
	_widgets.push_back(widget);
	
	_sumHeights += widget->height + 2*WIDGET_PADDING;
}

void scrWidgets::doResize()
{
	int x, y, w, h;
	getLiveBounds(x, y, w, h);
	
	_sumHeights = WIDGET_PADDING;
	
	FOREACH_WIDGET
	{
		_widgets[iWidget]->setBounds(x+WIDGET_PADDING, y+_sumHeights+WIDGET_PADDING,
										   w - 2*WIDGET_PADDING);
		_sumHeights += _widgets[iWidget]->height + 2*WIDGET_PADDING;
	}
}

void scrWidgets::drawContent()
{
	FOREACH_WIDGET
		_widgets[iWidget]->draw();
}