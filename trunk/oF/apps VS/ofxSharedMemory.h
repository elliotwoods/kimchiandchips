#include <vcclr.h>

#include "ofMain.h"

using namespace System;
using namespace VVVV::Utils::SharedMemory;

class ofxSharedMemory
{
public:
	ofxSharedMemory();
	~ofxSharedMemory();

	void		init(string name, long length);
	void		setData(unsigned char* data, long length);

private:
	gcroot<Segment^>						memShare;
	gcroot<cli::array< System::Byte >^ >	managedMem;

	bool	_isInitialised;
	int		_length;
};