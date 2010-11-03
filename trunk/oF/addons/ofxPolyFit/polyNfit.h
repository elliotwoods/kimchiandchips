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
#include "powerBases.h"

class polyNfit : public powerBases, public FitBase
{
public:
	polyNfit(int order, int indim, int outdim, int basesShape) :
 		_order(order),
		FitBase(updateBasisIndicies(indim,order), indim, outdim),
		powerBases(basesShape)
		{
		};

	virtual double		basis(int n, std::vector<double> &x);
	int					_order;
};