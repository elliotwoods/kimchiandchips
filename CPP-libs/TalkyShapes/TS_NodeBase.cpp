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

void TS_NodeBase::draw(TS_DrawBase *drawClass)
{
    map<unsigned long, TS_ShapeBase*>::iterator it;
    
    for (it = Shapes.begin(); it != Shapes.end(); it++)
        drawClass->drawShape( (*it).second );
    
}
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// SHAPE MANIPULATION
////////////////////////////////////////////////////////////////
//

int TS_NodeBase::addShape(TS_ShapeBase *shape)
{
    int ID = getNextID();
    
    Shapes[getNextID()] = shape;    
    
    return ID;
}

void TS_NodeBase::updateShape(TS_ShapeBase *shape)
{
    updateShape(shape, shape->ID);
}

void TS_NodeBase::updateShape(TS_ShapeBase *shape, int ID)
{
    TS_ShapeBase *oldShape;
    
    if (oldShape->Type != shape->Type)
    {
        TS_Error::passError(TS_ERROR__SHAPE_UPDATE_TYPE_MISMATCH);
        return;
    }
    
    //perhaps need to check number of vertices as well
    
    //delete old shape
    delete Shapes[ID];
    
    //and replace with new one
    Shapes[ID] = shape;
}

void TS_NodeBase::deleteShape(int ID)
{
    Shapes.erase(ID);
}
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// SHAPE DATA
////////////////////////////////////////////////////////////////
//

int TS_NodeBase::getClosestVertex(int ShapeID, Vector2f const &pipet, float radius)
{
    ////////////////////////////
    // MOVE TO SEPERATE FUNCTION
    //
    int countIDs = Shapes.count(ShapeID);
    
    if (countIDs > 1)
    {
        TS_Error::passError(TS_ERROR__SHAPE_ID_REDUNDANCY);
        return -1;
    }
    
    if (countIDs == 0)
    {
        TS_Error::passError(TS_ERROR__SHAPE_ID_NON_EXISTENT);
        return -1;
    }
    ////////////////////////////
    
    return Shapes[ShapeID]->getClosestVertex(pipet, radius);
}

Vector2f TS_NodeBase::getVertexXY(int ShapeID, int iVertex)
{
    return Shapes[ShapeID]->getVertexXY(iVertex);
}

void TS_NodeBase::moveVertex(int ID, int iVertex, Vector2f const &dXY)
{
    ////////////////////////////
    // MOVE TO SEPERATE FUNCTION
    //
    int countIDs = Shapes.count(ID);
    
    if (countIDs > 1)
    {
        TS_Error::passError(TS_ERROR__SHAPE_ID_REDUNDANCY);
        return;
    }
    
    if (countIDs == 0)
    {
        TS_Error::passError(TS_ERROR__SHAPE_ID_NON_EXISTENT);
        return;
    }
    ////////////////////////////
    
    Shapes[ID]->moveVertex(iVertex, dXY);
    
}

void TS_NodeBase::moveShape(int ID, Vector2f const &dXY)
{    
    ////////////////////////////
    // MOVE TO SEPERATE FUNCTION
    //
    int countIDs = Shapes.count(ID);
    
    
    if (countIDs > 1)
    {
        TS_Error::passError(TS_ERROR__SHAPE_ID_REDUNDANCY);
        return;
    }
    
    if (countIDs == 0)
    {
        TS_Error::passError(TS_ERROR__SHAPE_ID_NON_EXISTENT);
        return;
    }
    ////////////////////////////
    
    Shapes[ID]->moveShape(dXY);
    
}
//
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// CONNECTION INFO
////////////////////////////////////////////////////////////////
//

bool TS_NodeBase::getIsConnected()
{
    if (!isSetup())
    {
        TS_Error::passError(TS_ERROR__TALKY_NOT_INITIALISED);
        return false;
    }
    
    return TalkyNode->getIsConnected();
}

float TS_NodeBase::getTimeUntilNextConnectNorm()
{
    if (!isSetup())
    {
        TS_Error::passError(TS_ERROR__TALKY_NOT_INITIALISED);
        return false;
    }
    
    return TalkyNode->getTimeUntilNextConnectNorm();
}
//
////////////////////////////////////////////////////////////////


bool TS_NodeBase::isSetup()
{
    return nodeInitialised;
}

int TS_NodeBase::getNextID()
{
    int ID = Shapes.size();
    
    while (Shapes.count(ID) > 0)
        ID++;
    
    return ID;
}


