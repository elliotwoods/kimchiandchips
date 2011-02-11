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
    //
    //can it delete itself?
    //
    if (isSetup())
        delete TalkyNode;
}

void TS_NodeBase::draw(TS_DrawBase &drawClass)
{
    ShapesIterator it;
    
    for (it = Shapes.begin(); it != Shapes.end(); it++)
        drawClass.drawShape( (*it).second );
    
}

string TS_NodeBase::toString()
{
    stringstream out;
    
    ShapesIterator it;
    
    for ( it=Shapes.begin(); it != Shapes.end(); it++ )
        out << "Shapes[" << (*it).first << "] = { " << (*it).second->toString() << " }\n";
    
    return out.str();
}
//
////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
// SHAPE MANIPULATION
////////////////////////////////////////////////////////////////
//

void TS_NodeBase::clearShapes()
{
    while (Shapes.size() > 0)
    {
        delete Shapes.end()->second;
        
        Shapes.erase(Shapes.end());
    }
}

void TS_NodeBase::addShape(TalkyMessage &msg, bool forceUpdate)
{
    ////////////////////////
    // GET DATA IN
    ////////////////////////
    //
    int PayloadLength;
    char * Payload = msg.getPayload(PayloadLength);
    
    if (PayloadLength < 6)
    {
        TS_Error::passError(TS_ERROR__MSG_DESERIALISE_TOO_SHORT);
        return;
    }
    
    //ID
    unsigned long ID =  * (unsigned long *) Payload;
    //
    if (!forceUpdate)
        if (Shapes.count(ID) > 0)
        {
            TS_Error::passError(TS_ERROR__SHAPE_ID_REDUNDANCY);
            return;
        }
    //
    
    int Type = * (unsigned short *) (Payload + 4);
    ////////////////////////
    
    
    ////////////////////////
    // LOAD DATA IN SHAPE
    ////////////////////////
    //
    switch (Type) {
        case TS_Type_Quad:
        {
            ////////////////////////
            // CREATE A QUAD
            //
            TS_Quad *newQuad = new TS_Quad();
            
            newQuad->deSerialise(msg);
            
            //check if we should be overwriting
            //an existing shape
            if (forceUpdate && Shapes.count(newQuad->ID) > 0)
            {
                delete Shapes[ID];
                Shapes[ID] = newQuad;
            }
            else
                Shapes.insert (pair<unsigned long, TS_ShapeBase*> (ID, newQuad));

                
            
            cout << "add shape ID:" << newQuad->ID << "\n";
            
            break;
            //
            ////////////////////////
        }
            
        default:
            break;
    }
    ////////////////////////
}

int TS_NodeBase::addShape(TS_ShapeBase *shape, bool updateOther)
{    
    //////////////////
    // ADD TO MAP
    //////////////////
    //
    unsigned long ID = getNextID();
    shape->ID = ID;
    
    Shapes.insert (pair<unsigned long, TS_ShapeBase*> (ID, shape));
    //////////////////

    if (updateOther)
    {
        //////////////////
        // SEND TO OTHER
        //////////////////
        //
        TalkyMessage msg;
        
        Shapes[ID]->serialise(msg);
        msg.ContentsType = TS_MSG_SHAPE_INSERT;
        
        TalkyNode->send(msg);
        //////////////////
    }

    return ID;
}

void TS_NodeBase::updateShape(TalkyMessage &msg)
{
    int ID = * (int*) msg.getPayload();
    if (Shapes.count(ID) == 0)
    {
        TS_Error::passError(4000);
        return;
    }
    
    Shapes[ID]->deSerialise(msg);
}

void TS_NodeBase::updateShape(TS_ShapeBase *shape, bool updateOther)
{
    updateShape(shape, shape->ID, updateOther);
}

void TS_NodeBase::updateShape(TS_ShapeBase *shape, int ID, bool updateOther)
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
    
    if (updateOther)
        updateShape(ID);
}

void TS_NodeBase::updateShape(int ID)
{
    TalkyMessage msg;
    
    Shapes[ID]->serialise(msg);
    msg.ContentsType = TS_MSG_SHAPE_UPDATE;
    
    TalkyNode->send(msg);
    
}


void TS_NodeBase::deleteShape(TalkyMessage &msg)
{
    unsigned long ID = * (unsigned long*) msg.getPayload();
    
    deleteShape(ID, false);
}

void TS_NodeBase::deleteShape(unsigned long ID, bool updateOther)
{
    cout << "delete shape ID:" << ID << "\n";
    Shapes.erase(ID);
    
    if (updateOther)
    {
        TalkyMessage msg;
        
        msg.ContentsType = TS_MSG_SHAPE_DELETE;
        
        msg.setPayload((char*) &ID, 4);
        
        TalkyNode->send(msg);
    }
    
}

void TS_NodeBase::pushAll()
{   
    TalkyBundleMessage bundleMsg;
    TalkyMessage packedMsg;
    
    TalkyMessage individualMsg;
    
    ShapesIterator shape;
    for (shape = Shapes.begin(); shape != Shapes.end(); shape++)
    {
        shape->second->serialise(individualMsg);
        bundleMsg.push(individualMsg);
    }
    
    packedMsg = bundleMsg.pack();
    
    packedMsg.ContentsType = TS_MSG_SHAPES_PUSH_ALL;
    
    TalkyNode->send(packedMsg);
}

void TS_NodeBase::pullAll(TalkyMessage &msg)
{
    TalkyBundleMessage bundleMsg;
    bundleMsg.unpack(msg, TS_MSG_SHAPE_INSERT);
    
    clearShapes();
    
    for (int i=0; i<bundleMsg.bundle.size(); i++)
        addShape(bundleMsg.bundle[i], true);
    
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
    
    updateShape(ID);
    
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
    
    updateShape(ID);
    
}
//
////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
// CONNECTION
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

void TS_NodeBase::msgRxAvailable(const void* pSender, int &receiveQueueCount)
{
    processReceiveQueue();
}
//
////////////////////////////////////////////////////////////////


bool TS_NodeBase::isSetup()
{
    return nodeInitialised;
}

unsigned long TS_NodeBase::getNextID()
{
    unsigned long ID = Shapes.size();
    
    while (Shapes.count(ID) > 0)
        ID++;
    
    return ID;
}

void TS_NodeBase::processReceiveQueue()
{
    vector<TalkyMessage> *msgs = &(TalkyNode->receiveQueue);
    
    while (!msgs->empty()) {
        TalkyMessage msg = msgs->back();
        msgs->pop_back();
        
        cout << msg.toString();
        
        switch (msg.ContentsType) {
            
            case TS_MSG_SHAPES_PUSH_REQUEST:
                pushAll();
                break;
            
            case TS_MSG_SHAPES_PUSH_ALL:
                pullAll(msg);
                break;
                
            case TS_MSG_SHAPE_INSERT:
                addShape(msg);
                break;
            
            case TS_MSG_SHAPE_UPDATE:
                updateShape(msg);
                break;
                
            case TS_MSG_SHAPE_DELETE:
                deleteShape(msg);
                break;
                
            default:
                break;
        }
    }
}
