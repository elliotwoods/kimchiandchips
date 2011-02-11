//
//  TS_Error.h
//  TalkyShapes
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_ErrorHandler_Default.h"
#include "vmath.h"

using namespace std;

class TS_Error
{
public:
    static void setErrorHandler(TS_ErrorHandler *handler);
    static void passError(int errorCode);
    
    static Vector2f debugXY;
protected:
    static TS_ErrorHandler      *errorHandler;
};