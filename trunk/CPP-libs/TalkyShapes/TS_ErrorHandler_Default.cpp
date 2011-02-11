//
//  TS_ErrorHandler_Default.cpp
//  TalkyShapes
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_ErrorHandler_Default.h"

void TS_ErrorHandler::throwError(int errorCode)
{
    string errString = getErrorString(errorCode);
    throw(errString);
    
    //maybe you might want to do, for example
    //in openFrameworks
    //ofError(OF_LOG_ERROR, getErrorString(errorCode)
}

string TS_ErrorHandler::getErrorString(int errorCode)
{
    switch (errorCode) {
            
        case TS_ERROR__TALKY_NOT_INITIALISED:
            return string("TalkyNode pointer not intitialised, have you run setup on this TalkyShapes node yet?");
        
        case TS_ERROR__SHAPE_DRAW_NOT_IMPLEMENTED:
            return "Your Draw class (i.e. that inherits TS_DrawBase) doesn't support this shape type";
        
        case TS_ERROR__DESERIALISE_MISMATCH_PLURALITY_SINGLE:
            return "Cannot deserialise this shape message to this shape. Please send me a single object";
        
        case TS_ERROR__DESERIALISE_MISMATCH_MESSAGE_LENGTH:
            return "Cannot deserialise this shape message to this shape. Message length is incorrect";

        case TS_ERROR__DESERIALISE_MISMATCH_TERMINATOR:
            return "Cannot deserialise this shape message to this shape. Terminator seems incorrect. Perhaps data is corrupt or malformed";
            
        case TS_ERROR__DESERIALISE_MISMATCH_SHAPE_TYPE:
            return "Cannot deserialise this shape message to this shape. Shape types do not match";
            
        case TS_ERROR__DESERIALISE_MISMATCH_NVERTICIES:
            return "Cannot deserialise this shape message to this shape. Number of vertices do not match";
        
        case TS_ERROR__MSG_DESERIALISE_TOO_SHORT:
            return "Cannot deserialise message to shape, too short";
        
        case TS_ERROR__SHAPE_ID_NON_EXISTENT:
            return "Cannot select shape as shape ID does not exist in dataset";
            
        case TS_ERROR__SHAPE_ID_REDUNDANCY:
            return "Cannot select shape as shape ID's are non-unique. Check shape dataset for redundant ID's";
            
        case TS_ERROR__SHAPE_VERTICES_NOT_INITIALISED:
            return "Shape's vertices not intialised";

        case TS_ERROR__SHAPE_VERTEX_INDEX_OUT_RANGE:
            return "Shape's vertex index out of range";
            
        case TS_ERROR__SHAPE_UPDATE_TYPE_MISMATCH:
            return "Cannot update shape with this one. They're of different types";
            
        case TS_ERROR__SHAPE_ADD_ID_ALREADY_EXISTS:
            return "Can't add this shape, we already have this ID stored locally. You probably want to use addShape(msg, forceUpdate=true)";
            
        default:
            break;
    }
    
    return "";
}