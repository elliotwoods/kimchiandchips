#pragma once
///////////////////////////////////////////////////////////////////////////////
//
// Class to solve Ax = y for two values of y, where A is a matrix made up
// of bais functions a_ij = sum_x f_i(x)f_j(x). As is the case when
// finding the least squares fit polynomial.
//
// (c) 2009 Daniel Tang
//
///////////////////////////////////////////////////////////////////////////////

#include "matrix.h"
#include <vector>

class FitBase
{
public:
	FitBase(int bases, int indim, int outdim);
	~FitBase();
	
	void				init(std::vector<std::vector<double> > points, std::vector<std::vector<double> > data, int M);
	void				solve(int, double *, int &nCoefficients);
	int					nBases() { return _bases; };
protected:
	
	// Solves Ax = y for x
	Matrix								A;	
	std::vector<std::vector<double> >	y;	// store OUTDIM values of y
	
	virtual double						basis(int n, std::vector<double> &x) = 0;
	
	int									_bases;
	int									_indim, _outdim;
};