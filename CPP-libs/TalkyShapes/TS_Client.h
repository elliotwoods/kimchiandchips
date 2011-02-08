#pragma once
//
//  TS_Client.h
//  Map Tools
//
//  Created by Elliot Woods on 06/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TS_NodeBase.h"

class TS_Client : public TS_NodeBase
{
    public:
        void    setup(TalkyBase *talkyClient, string remoteHost);
    
        void    requestUpdates();
    
};