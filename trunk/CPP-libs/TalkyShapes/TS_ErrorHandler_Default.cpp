//
//  TS_ErrorHandler_Default.cpp
//  iPadExample
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_ErrorHandler_Default.h"

void TS_ErrorHandler::throwError(int errorCode)
{
    throw(getErrorString(errorCode));
    
    //maybe you might want to do, for example
    //in openFrameworks
    //ofError(OF_LOG_ERROR, getErrorString(errorCode)
}

string TS_ErrorHandler::getErrorString(int errorCode)
{
    switch (errorCode) {
            
        case TS_ERROR__TALKY_NOT_INITIALISED:
            return "TalkyNode pointer not intitialised, have you run setup on this TalkyShapes node yet?";
            
        default:
            break;
    }
    
    return "";
}