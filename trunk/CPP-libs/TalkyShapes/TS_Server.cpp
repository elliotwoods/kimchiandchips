//
//  TS_Server.cpp
//  Map Tools
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_Server.h"

void TS_Server::setup(TalkyBase *talkyServer)
{
    //setup the server
    this->TalkyNode = talkyServer;
    TalkyNode->setup(TS_NETWORK_PORT);
    
    nodeInitialised = true;
}

bool TS_Server::getIsServerBound()
{
    if (!isSetup())
    {
        TS_Error::passError(TS_ERROR__TALKY_NOT_INITIALISED);
        return false;
    }
    
    return TalkyNode->getIsServerBound();
}

int TS_Server::getNumClients()
{
    if (!isSetup())
    {
        TS_Error::passError(TS_ERROR__TALKY_NOT_INITIALISED);
        return 0;
    }
    
    return TalkyNode->getNumClients();
}