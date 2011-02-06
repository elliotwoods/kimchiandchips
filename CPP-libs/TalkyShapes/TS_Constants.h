#pragma once
/*
 *  TS_Constants.h
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include <string>

#define TS_NETWORK_PORT 4896

#define TS_MSG_RANGE_OBJECT_SINGLE_MIN 30
#define TS_MSG_RANGE_OBJECT_SINGLE_MAX 39

#define TS_MSG_RANGE_OBJECT_BUNDLE_MIN 20
#define TS_MSG_RANGE_OBJECT_BUNDLE_MAX 25

#define TS_SHAPE_TYPE_QUAD 3

#define TS_PAYLOAD_TERMINATOR 127


#define TS_ERROR__TALKY_NOT_INITIALISED 1001

using namespace std;

namespace TalkyShapes {
    
    string getErrorString(int errorCode)
    {
        switch (errorCode) {
                
            case TS_ERROR__TALKY_NOT_INITIALISED:
                return "TalkyNode pointer not intitialised, have you run setup on this TalkyShapes node yet?";
                
            default:
                break;
        }
        
        return "";
    }
}