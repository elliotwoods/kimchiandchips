/*
 *  powerBases.cpp
 *  polyncpp-test
 *
 *  Created by Elliot Woods on 26/03/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "Stdafx.h"
#include "powerBases.h"

powerBases::powerBases(int basesShape)
{
	_basesShape=basesShape;
}

int powerBases::updateBasisIndicies(int nDimensions, int nPowerOrder)
{
	_nDimensions = nDimensions;
	_nPowerOrder = nPowerOrder;
	
	if (_nDimensions != _renderedNDimensions
		|| _nPowerOrder != _renderedNPowerOrder)
	{
		calc();
		_renderedNDimensions = _nDimensions;
		_renderedNPowerOrder = _nPowerOrder;
	}
	
	return _nBases;
}

void powerBases::calc()
{

	if (_basesShape < BASIS_SHAPE_PADE_FIRST)
	{
		// create temporary variables for each item
		int *iPossibleItemX = new int[_nDimensions];
		bool present;
		
		//
		// clean out bases
		//
		clear();
		
		//
		// iterate through all possible items for square/cubic/etc
		//
		for (int iPossibleItem=0; iPossibleItem < pow(_nPowerOrder+1,(double)_nDimensions); iPossibleItem++)
		{
			
			//
			// calculate indicies in each dimension for this item
			//
			int iOrderSum=0;
			
			for (int iDimension=0; iDimension < _nDimensions; iDimension++)
			{
				iPossibleItemX[iDimension] = iPossibleItem 
				/ (int)pow(_nPowerOrder+1,(double)iDimension)
				% (_nPowerOrder+1);
				
				if (iDimension<_nDimensions-1 || _basesShape==BASIS_SHAPE_SQUARE_MINUS_HIGHEST)
					iOrderSum += iPossibleItemX[iDimension];
			}
			
			
			//
			// determine whether current index should be selected
			//
			switch(_basesShape)
			{
			case BASIS_SHAPE_TRIANGLE:
				present = iOrderSum < (_nPowerOrder+1) - iPossibleItemX[_nDimensions-1];
				break;

			case BASIS_SHAPE_SQUARE:
				present = true;
				break;

			case BASIS_SHAPE_SQUARE_MINUS_HIGHEST:
				present = iOrderSum < _nDimensions;
				break;
			}
			
			
			//
			// Fill array with data indicies
			//
			
			int idxBasisIndicies;
			if (present)
			{
				vecBasisIndicies.push_back(new unsigned int[_nDimensions]);
				idxBasisIndicies = vecBasisIndicies.size()-1;
				
				for (int iDimension=0; iDimension<_nDimensions; iDimension++)
				{
					vecBasisIndicies.at(idxBasisIndicies)[iDimension]=iPossibleItemX[iDimension];
					
				}
			}		
		}
		
		_nBases = vecBasisIndicies.size();
		
		_renderedNDimensions = _nDimensions;
		_renderedNPowerOrder = _nPowerOrder;
	} else if (_basesShape == BASIS_SHAPE_PADE_FIRST ) {

		//
		// PADE BASES 1st order
		//

		_nDimensions=4;
		vecBasisIndicies.clear();
		for (int iBasis=0; iBasis<7; iBasis++)
		{
			vecBasisIndicies.push_back(new unsigned int[_nDimensions]);
			for (int iDimension=0; iDimension<_nDimensions; iDimension++)
				vecBasisIndicies[iBasis][iDimension]=0;
		}

		//E x x'
		vecBasisIndicies[0][0]=1;
		vecBasisIndicies[0][3]=1;

		//F y x'
		vecBasisIndicies[1][1]=1;
		vecBasisIndicies[1][3]=1;

		//G z x'
		vecBasisIndicies[2][2]=1;
		vecBasisIndicies[2][3]=1;

		//A x
		vecBasisIndicies[3][0]=1;

		//B y
		vecBasisIndicies[4][1]=1;

		//C z
		vecBasisIndicies[5][2]=1;

		//D

		//constant
		//

		_nBases = vecBasisIndicies.size();
		
		_renderedNDimensions = _nDimensions;
		_renderedNPowerOrder = _nPowerOrder;
	} else if (_basesShape == BASIS_SHAPE_PADE_SECOND ) {

		//
		// PADE BASES 2nd order
		// 14 coefficients
		//

		_nDimensions=4;
		vecBasisIndicies.clear();
		for (int iBasis=0; iBasis<14; iBasis++)
		{
			vecBasisIndicies.push_back(new unsigned int[_nDimensions]);
			for (int iDimension=0; iDimension<_nDimensions; iDimension++)
				vecBasisIndicies[iBasis][iDimension]=0;
		}

		//0 A x
		vecBasisIndicies[0][0]=1;

		//1 B y
		vecBasisIndicies[1][1]=1;

		//2 C z
		vecBasisIndicies[2][2]=1;

		//3 D x x
		vecBasisIndicies[3][0]=2;

		//4 E y y
		vecBasisIndicies[4][1]=2;

		//5 F z z
		vecBasisIndicies[5][2]=2;

		//6 G x y
		vecBasisIndicies[6][0]=1;
		vecBasisIndicies[6][1]=1;

		//7 H y z
		vecBasisIndicies[7][1]=1;
		vecBasisIndicies[7][2]=1;

		//8 I z x
		vecBasisIndicies[8][0]=1;
		vecBasisIndicies[8][2]=1;

		//9 J x y z
		vecBasisIndicies[9][0]=1;
		vecBasisIndicies[9][1]=1;
		vecBasisIndicies[9][2]=1;


		//10 K - 
		//constant
		//

		//11 L x x'
		vecBasisIndicies[11][0]=1;
		vecBasisIndicies[11][3]=1;

		//12 M y x'
		vecBasisIndicies[12][1]=1;
		vecBasisIndicies[12][3]=1;

		//13 N z x'
		vecBasisIndicies[13][2]=1;
		vecBasisIndicies[13][3]=1;

		_nBases = vecBasisIndicies.size();
		
		_renderedNDimensions = _nDimensions;
		_renderedNPowerOrder = _nPowerOrder;
	}
}

void powerBases::clear()
{
	for (int iBasis=0; iBasis < vecBasisIndicies.size(); iBasis++)
		delete[] vecBasisIndicies[iBasis];

	vecBasisIndicies.clear();
	_nBases=0;
}