//
//  TS_Error.cpp
//  iPadExample
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_Error.h"

Vector2f TS_Error::debugXY = Vector2f(0, 0);


//////////////////////////////////////
//intialise with default error handler
//
TS_ErrorHandler* TS_Error::errorHandler = new TS_ErrorHandler();
//
//if you want to perform your own error
//throwing for your own platform,
//then just overwrite this errorHandler
//with TS_Error::setErrorHandler(..) function
//and an instance of your own class
//
//you own class must inherit TS_ErrorHandler
//which is inside TS_ErrorHandler_Default.h
//////////////////////////////////////

void TS_Error::setErrorHandler(TS_ErrorHandler *handler)
{
    delete errorHandler;
    
    errorHandler = handler;
}

void TS_Error::passError(int errorCode)
{
    errorHandler->throwError(errorCode);
}

