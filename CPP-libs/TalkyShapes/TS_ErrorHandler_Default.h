#pragma once
//
//  TS_ErrorHandler_Default.h
//  iPadExample
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include <string>

#include "TS_Constants.h"
using namespace std;

class TS_ErrorHandler
{
    public:
        virtual void throwError(int errorCode);
    protected:
        string getErrorString(int errorCode);
};