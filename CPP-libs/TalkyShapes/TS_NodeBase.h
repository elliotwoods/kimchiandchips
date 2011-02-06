/*
 *  TS_NodeBase.h
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TS_Quad.h"
#include "TS_ShapeBase.h"

#include "TalkyBase.h"
#include "TS_Constants.h"

using namespace TalkyShapes;

class TS_NodeBase
{
    public:
        ~TS_NodeBase();
    
        vector<TS_ShapeBase*>  Shapes;
        
        void    addShape(TS_ShapeBase *shape); // assigns ID

        void    updateShape(TS_ShapeBase *shape); //looks up ID
        void    updateShape(TS_ShapeBase *shape, int ID); //uses ID
        
        void    deleteShape(int ID);

        bool	getIsConnected();
        float	getTimeUntilNextConnectNorm();

    protected:
        bool    isSetup();
    
        //feel free to override with sensible error handling
        //for your platform
        virtual void    throwError(int errorCode);

        TalkyBase      *TalkyNode;
        bool            nodeInitialised;
};