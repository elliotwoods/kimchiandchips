/*
 *  scrFileSelect.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 04/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrFileSelect.h"

scrFileSelect::scrFileSelect(string caption, string path, string extension) :
scrWidgets(caption),
nFiles(_nFiles)
{
	_path = path;
	allowExt(extension);
	selected = new bool[1];
	
	refresh();
}

void scrFileSelect::refresh()
{
	_nFiles = listDir(_path);
	
	for (int i=0; i<_widgets.size(); i++)
		delete _widgets[i];
	
	delete[] selected;
	selected = new bool[_nFiles];
	
	for (int i=0; i<_nFiles; i++)
	{
		selected[i]=false;
		push(new wdgButton(getName(i),
						   selected[i]));
	}
	
}