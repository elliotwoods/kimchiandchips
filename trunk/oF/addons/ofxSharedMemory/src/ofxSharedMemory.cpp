#include "ofxSharedMemory.h"

ofxSharedMemory::ofxSharedMemory()
{
	_isInitialised = false;
}

ofxSharedMemory::~ofxSharedMemory()
{
}

void ofxSharedMemory::init(string name, long length)
{
	_length = length;
	memShare = gcnew Segment(gcnew String(name.c_str()), SharedMemoryCreationFlag::Create, length+SHARED_MEM_FUDGE_OFFSET);
	managedMem = gcnew cli::array< System::Byte >(length);

	_isInitialised = true;
}

void ofxSharedMemory::setData(unsigned char* data, long length)
{
	if (!_isInitialised)
	{
		ofLog(OF_LOG_WARNING, "ofxSharedMemory: Cant call setData until init is called");
		return;
	}

	Runtime::InteropServices::Marshal::Copy(
				(System::IntPtr)(data+SHARED_MEM_FUDGE_OFFSET),
				managedMem,
				0, _length);

	memShare->Lock();
	memShare->SetData(managedMem);	
	memShare->Unlock();

}