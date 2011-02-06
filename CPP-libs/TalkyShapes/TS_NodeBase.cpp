/*
 *  TS_NodeBase.cpp
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TS_NodeBase.h"

TS_NodeBase::~TS_NodeBase()
{
    //can we cleanly delete on a base type?
    //i.e. with a smaller byte size
    //perhaps consider move to server,client
    //destructors
    //
    if (isSetup())
        delete TalkyNode;
}

void TS_NodeBase::updateShape(TS_ShapeBase *shape)
{
    updateShape(shape, shape->ID);
}

bool TS_NodeBase::getIsConnected()
{
    if (!isSetup())
    {
        throwError(TS_ERROR__TALKY_NOT_INITIALISED);
        return false;
    }
    
    return TalkyNode->getIsConnected();
}

float TS_NodeBase::getTimeUntilNextConnectNorm()
{
    if (!isSetup())
    {
        throwError(TS_ERROR__TALKY_NOT_INITIALISED);
        return false;
    }
    
    return TalkyNode->getTimeUntilNextConnectNorm();
}

bool TS_NodeBase::isSetup()
{
    return nodeInitialised;
}

void TS_NodeBase::throwError(int errorCode)
{
    throw("TalkyShapes node: " + TalkyShapes::getErrorString(errorCode));
}
