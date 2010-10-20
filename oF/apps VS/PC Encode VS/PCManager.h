#pragma once

/*
 *  PCManager.h
 *  PCDecode
 *
 *  Created by Elliot Woods on 25/10/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofMain.h"

#include "PCEncode.h"

class PCManager : public PCConfig 
{
private:
		PCLogger			*_logger;
		PCEncode			*_encoder;
		PCDecode			*_decoder;
};