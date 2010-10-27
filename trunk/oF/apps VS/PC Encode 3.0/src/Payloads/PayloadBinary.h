/*
 *  PayloadBinary.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PayloadBase.h"

#include "ofMain.h"

class PayloadBinary : public PayloadBase
{
public:
	void			setup(); // calc vars, data, error check

	bool			decode(int reading, int &iX, int &iY);
};