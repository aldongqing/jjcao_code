//===========================================================================
// GoTools - SINTEF Geometry Tools version 1.0.1
//
// GoTools module: CORE
//
// Copyright (C) 2000-2005 SINTEF ICT, Applied Mathematics, Norway.
//
// This program is free software; you can redistribute it and/or          
// modify it under the terms of the GNU General Public License            
// as published by the Free Software Foundation version 2 of the License. 
//
// This program is distributed in the hope that it will be useful,        
// but WITHOUT ANY WARRANTY; without even the implied warranty of         
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          
// GNU General Public License for more details.                           
//
// You should have received a copy of the GNU General Public License      
// along with this program; if not, write to the Free Software            
// Foundation, Inc.,                                                      
// 59 Temple Place - Suite 330,                                           
// Boston, MA  02111-1307, USA.                                           
//
// Contact information: e-mail: tor.dokken@sintef.no                      
// SINTEF ICT, Department of Applied Mathematics,                         
// P.O. Box 124 Blindern,                                                 
// 0314 Oslo, Norway.                                                     
//
// Other licenses are also available for this software, notably licenses
// for:
// - Building commercial software.                                        
// - Building software whose source code you wish to keep private.        
//===========================================================================
#ifndef _LUDECOMP_IMPLEMENTATION_H
#define _LUDECOMP_IMPLEMENTATION_H

#include <vector>
#include <cmath>

namespace Go 
{
///\addtogroup utils
///\{


//===========================================================================
// LU decomposition algorithm, based on Crout's algorithm
template<typename SquareMatrix> 
void LUDecomp(SquareMatrix& mat, int num_rows, int* perm, bool& parity)
//===========================================================================
{
    parity = true; // as for now, number of row transpositions is 0, evidently
    const int num_cols = num_rows; // for clarity (should be optimized away by compiler...)

    for (int k = 0; k < num_rows; perm[k] = k++); // filling out perm with sequence 0, 1,...

    // determining scaling factor of each row
    std::vector<double> scaling(num_rows, 0);
    for (int i = 0; i < num_rows; ++i) {
	for (int j = 0; j < num_cols; ++j) {
	    double temp = fabs(mat[i][j]);
	    if (temp > scaling[i]) {
		scaling[i] = temp; // scaling[i] is max. absolute elem on row i.
	    }
	}
	if (scaling[i] == 0) {
	    throw std::runtime_error("Unable to LU decompose matrix.  Null row detected.");
	} else {
	    scaling[i] = double(1) / scaling[i];
	}
     }

    // executing Crout's algorithm
    for (int j = 0; j < num_cols; ++j) {
	// determining elements of UPPER matrix on this column
	for (int i = 0; i < j; ++i) {
	    double sum = mat[i][j];
	    for (int k = 0; k <= i-1; ++k) {
		sum -= mat[i][k] * mat[k][j];
	    }
	    mat[i][j] = sum;
	}

	// compute rest of this column, before division by pivot
	double pivot_val = 0;
	int pivot_row = j;
	for (int i = j; i < num_rows; ++i) {
	    double sum = mat[i][j];
	    for (int k = 0; k <= j-1; ++k) {
		sum -= mat[i][k] * mat[k][j];
	    }
	    mat[i][j] = sum;
	    double temp = std::fabs(sum * scaling[i]);
	    if (temp > pivot_val) {
		pivot_val = temp;
		pivot_row = i;
	    }
	}

	if (mat[pivot_row][j] == 0) {
	    throw std::runtime_error("Unable to LU decompose singular matrix.");
	}

	// permute rows to position pivot correctly
	if (pivot_row != j) {
	    for (int k = 0; k < num_cols; ++k) {
		std::swap(mat[pivot_row][k], mat[j][k]);
	    }
	    parity = !parity;
	    std::swap(scaling[j], scaling[pivot_row]);
	    std::swap(perm[j], perm[pivot_row]);
	}
	
	if (j < num_rows - 1) {
	    // dividing LOWER matrix elements by pivot
	    pivot_val = double(1) / mat[j][j]; // inverse value, without scaling
	    for (int i = j+1; i < num_rows; ++i) {
		mat[i][j] *= pivot_val; 
	    }
	}
    }
}

//===========================================================================
// Solve the system Ax = b for x, using LU decomposition of the matrix A.
template<typename SquareMatrix, typename T>
void LUsolveSystem(SquareMatrix& A, int num_unknowns, T* vec)
//===========================================================================
{
    bool parity;
    std::vector<int> permutation(num_unknowns);
    
    LUDecomp(A, num_unknowns, &permutation[0], parity);

    // permuting b
    std::vector<T> vec_old(vec, vec + num_unknowns);
    for (int i = 0; i < num_unknowns; ++i) {
	swap(vec[i], vec_old[permutation[i]]);
    }
    forwardSubstitution(A, vec, num_unknowns);
    backwardSubstitution(A, vec, num_unknowns);
}

//===========================================================================
template<typename SquareMatrix, typename T>
void forwardSubstitution(const SquareMatrix& A, T* x, int num_unknowns)
//===========================================================================
{
    for (int i = 1; i < num_unknowns; ++i) {
	for (int j = 0; j < i; ++j) {
	    x[i] -= A[i][j] * x[j];
	}
    }
}

//===========================================================================
template<typename SquareMatrix>
void forwardSubstitution(const SquareMatrix& A, std::vector<double>* x, int num_unknowns)
//===========================================================================
{
    const int dim = int(x[0].size());
    for (int i = 1; i < num_unknowns; ++i) {
	for (int j = 0; j < i; ++j) {
	    for (int dd = 0; dd < dim; ++dd) {
		x[i][dd] -= A[i][j] * x[j][dd];
	    }
	}
    }
}

//===========================================================================
template<typename SquareMatrix, typename T>
void backwardSubstitution(const SquareMatrix& A, T* x, int num_unknowns)
//===========================================================================
{
    x[num_unknowns-1] /= A[num_unknowns-1][num_unknowns-1];
    for (int i = num_unknowns - 2; i >= 0; --i) {
	for (int j = i+1; j < num_unknowns; ++j) {
	    x[i] -= A[i][j] * x[j];
	}
	x[i] /= A[i][i];
    }
}

//===========================================================================
template<typename SquareMatrix>
void backwardSubstitution(const SquareMatrix& A, std::vector<double>* x, int num_unknowns)
//===========================================================================
{
    const int dim = int(x[0].size());
    for (int dd = 0; dd < dim; ++dd) {
	x[num_unknowns-1][dd] /= A[num_unknowns-1][num_unknowns-1];
    }
    for (int i = num_unknowns - 2; i >= 0; --i) {
	for (int j = i+1; j < num_unknowns; ++j) {
	    for (int dd = 0; dd < dim; ++dd) {
		x[i][dd] -= A[i][j] * x[j][dd];
	    }
	}
	for (int dd = 0; dd < dim; ++dd) {
	    x[i][dd] /= A[i][i];
	}
    }
}

///\}
}; // end namespace Go

#endif // _LUDECOMP_IMPLEMENTATION_H


