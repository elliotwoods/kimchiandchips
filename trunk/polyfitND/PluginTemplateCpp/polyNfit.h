#pragma once
/*
 *  polyNfit.h
 *  polyncpp-test
 *
 *  Created by Elliot Woods on 28/03/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "FitBase.h"
#include "basisTransformIndicies.h"

class polyNfit : public basisTransformIndicies, public FitBase
{
public:
	polyNfit(int order, int indim, int outdim) :
 		_order(order),
		FitBase(updateBasisIndicies(indim,order), indim, outdim)
		{ };

	
protected:
	virtual double		basis(int n, std::vector<double> &x);
	int					_order;
};