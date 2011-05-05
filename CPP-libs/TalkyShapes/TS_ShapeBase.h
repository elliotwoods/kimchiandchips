#pragma  once
/*
 *  TS_ShapeBase.h
 *  TalkyShapes
 *
 *  Created by Elliot Woods on 04/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TS_Includes.h"

#include <vector>
#include <string>
#include <sstream>

#include "TalkyMessage.h"

using namespace std;

class TS_ShapeBase
{
public:
	TS_ShapeBase();
	~TS_ShapeBase();
	
    ////////////////////////
    // VIRTUALS
    ////////////////////////
    //
    virtual void    init(TS_ShapePalette *Palette, float x, float y, float scale) = 0;
    virtual void	serialise(TalkyMessage &msg) = 0;
	virtual void	deSerialise(TalkyMessage const &msg) = 0;
    
    virtual bool    isHit(Vector2f XY) = 0;
    //
    ////////////////////////
    
    void            init(TS_ShapePalette *Palette, Vector2f XY, float scale);
    
	int		getNVertices();
	int		getNVerticesX();
	int		getNVerticesY();
	
    Vector2f*       getVerticesPointer();
	Vector2f*       getVerticesPointer(int &nVertices);
    
    int             getClosestVertex(Vector2f const pipet, float radius=99999);
    Vector2f        getVertexXY(int iVertex);
    
    void            moveVertex(int iVertex, Vector2f const &dXY);
    void            moveShape(Vector2f const &dXY);
    
    virtual Vector2f getCenter();
    
    string          toString();
    
	unsigned long			ID;
    TS_ShapePalette         *Palette;
	TS_ShapeType			Type;
	vector<unsigned short>	Tags;
	
	
protected:
	int				getNBytesBase();
	int				getNBytesTotal();
	virtual int		getNBytesVertices();
	
	void			initialiseVertices(int count = -1); //-1 will trigger getNVertices()
	
	unsigned char	nVerticesX;
	unsigned char	nVerticesY;
	bool			nVerticesFixed;
	
	Vector2f        *vertices;
	int				verticesInitialised;
};
