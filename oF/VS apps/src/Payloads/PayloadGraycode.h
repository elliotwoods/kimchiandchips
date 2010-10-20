/*
 *  PayloadGraycode.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PayloadBase.h"

#include "ofMain.h"

class PayloadGraycode : public PayloadBase
{
public:
	void			setup(); // calc vars, data, error check

	void			decode(int reading, int &iX, int &iY, bool &valid);
};