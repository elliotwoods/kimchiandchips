#pragma once
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
#include "TS_DrawBase.h"

#include "TalkyBase.h"
#include "TS_Includes.h"

#include <map>

class TS_NodeBase
{
    public:
        ~TS_NodeBase();
    
        map<unsigned long, TS_ShapeBase*>  Shapes;
    
        void    draw(TS_DrawBase *drawClass);
        
        ///////////////////////
        // SHAPE MANIPULATION    
        //
        int     addShape(TS_ShapeBase *shape); // assigns ID

        void    updateShape(TS_ShapeBase *shape); //looks up ID
        void    updateShape(TS_ShapeBase *shape, int ID); //uses ID
        
        void    deleteShape(int ID);
        ///////////////////////
    
        ///////////////////////
        // SHAPE DATA
        int         getClosestVertex(int ShapeID, Vector2f const &pipet, float radius = 99999);
        Vector2f    getVertexXY(int ShapeID, int iVertex);
    
        void    moveVertex(int ID, int iVertex, Vector2f const &dXY);
        void    moveShape(int ID, Vector2f const &dXY);
        ///////////////////////
    
        ///////////////////////
        // CONNECTION INFO
        //
        bool	getIsConnected();
        float	getTimeUntilNextConnectNorm();
        ///////////////////////

    protected:
        bool    isSetup();
        
        int     getNextID();

        TalkyBase      *TalkyNode;
        bool            nodeInitialised;
};