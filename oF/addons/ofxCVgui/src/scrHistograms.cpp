/*
 *  scrHistograms.cpp
 *  pixel correlation
 *
 *  Created by Elliot Woods on 27/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "GUI.h"

scrHistograms::scrHistograms(int iScreen, enumShowCursor showCursor, bool hasCursorEvents, int screenOrder, string caption)
: scrBase(iScreen, showCursor, hasCursorEvents, screenOrder, enum_histogram, caption)
{
}

void scrHistograms::draw(int x, int y, int width, int height)
{
	int nHistograms = _vecHistograms.size();
	if (nHistograms>0)
	{
		int histogramHeight = height/nHistograms;
		
		for (int iHistogram = 0; iHistogram < nHistograms; iHistogram++)
		{
			_vecHistograms.at(iHistogram)->draw(x, y + (histogramHeight*iHistogram), width, histogramHeight);
		}
	}
	
}

void scrHistograms::draw()
{
	draw(_x, _y, _width, _height);
}

void scrHistograms::addHistogram(Histogram &histogram)
{
	_vecHistograms.push_back(&histogram);
}

void scrHistograms::mouseDown(float xX, float xY, int x, int y)
{
	scrBase::mouseDown(xX, xY, x, y);
	
	int nHistograms = _vecHistograms.size();

	if (nHistograms>0)
	{
		int iHistogram = xY/float(nHistograms);
		xY = xY*float(nHistograms);
		xY -= floor(xY);
		
		_vecHistograms.at(iHistogram)->mouseDown(xX,xY);
		
	}
}