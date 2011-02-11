#pragma once
/*
 *  TS_NodeBase.h
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TS_Includes.h"
#include "TS_Quad.h"
#include "TS_ShapeBase.h"
#include "TS_DrawBase.h"

#include "TalkyBase.h"
#include "TalkyBundleMessage.h"


#include <map>
#include <sstream>

using namespace std;

typedef map<unsigned long, TS_ShapeBase*>::iterator ShapesIterator;
typedef map<unsigned long, TS_ShapeBase*>::reverse_iterator ShapesReverseIterator;

class TS_NodeBase
{
    public:
        ~TS_NodeBase();
    
        map<unsigned long, TS_ShapeBase*>  Shapes;
    
        void    draw(TS_DrawBase &drawClass);
        string  toString();
        
        ///////////////////////
        // SHAPE MANIPULATION    
        //
    
        //clear
        void    clearShapes();
    
        //add
        void    addShape(TalkyMessage &msg, bool forceUpdate=false); //rx
        int     addShape(TS_ShapeBase *shape, bool updateOther=true); // assigns ID
    
    
        //update
        void    updateShape(TalkyMessage &msg);
        void    updateShape(TS_ShapeBase *shape, bool updateOther=true); //looks up ID
        void    updateShape(TS_ShapeBase *shape, int ID, bool updateOther=true); //uses ID
        void    updateShape(int ID); // push updates to counterpart
    
        //push all
        void    pushAll();
        void    pullAll(TalkyMessage &msg);

        
        //delete
        void    deleteShape(TalkyMessage &msg);
        void    deleteShape(unsigned long ID, bool updateOther=true);
    
        //push all
        ///////////////////////
    
        ///////////////////////
        // SHAPE DATA
        int         getClosestVertex(int ShapeID, Vector2f const &pipet, float radius = 99999);
        Vector2f    getVertexXY(int ShapeID, int iVertex);
    
        void    moveVertex(int ID, int iVertex, Vector2f const &dXY);
        void    moveShape(int ID, Vector2f const &dXY);
        ///////////////////////
    
        ///////////////////////
        // CONNECTION
        //
        bool	getIsConnected();
        float	getTimeUntilNextConnectNorm();
    
        void    msgRxAvailable(const void* pSender, int &receiveQueueCount);
        ///////////////////////

    protected:
        bool            isSetup();
        
        unsigned long   getNextID();
    
        void            processReceiveQueue();

        TalkyBase      *TalkyNode;
        bool            nodeInitialised;
};