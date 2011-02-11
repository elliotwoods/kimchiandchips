//
//  TS_Client.cpp
//  Map Tools
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_Client.h"

void TS_Client::setup(TalkyBase *talkyClient, string remoteHost)
{
    //setup the client
    this->TalkyNode = talkyClient;
    TalkyNode->setup(remoteHost, TS_NETWORK_PORT);
    
    nodeInitialised = true;
    
    //update from server
    requestUpdates();
}

void TS_Client::requestUpdates()
{
    TalkyMessage msg;
    
    msg.ContentsType = TS_MSG_SHAPES_PUSH_REQUEST;
    
    TalkyNode->send(msg);
}