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
#ifndef _GOPROJECTCURVEANDTAN_
#define _GOPROJECTCURVEANDTAN_

#include <boost/smart_ptr.hpp>

#include "Point.h"
#include "EvalCurveSet.h"
#include "SplineCurve.h"
#include "SplineSurface.h"



namespace Go
{
///\addtogroup geometry
///\{


/// Evaluator class representing the projection and and tangent curve given
/// a set of input curves.
/// In total 3 points are computed: the parameter point of the projection on
/// an input surface, the corresponding space pt and the projected cross tangent
/// point.
class ProjectCurveAndCrossTan : public EvalCurveSet
{
public:

    /// Constructor.
    /// \param space_crv the input space curve to project.
    /// \param crosstan_crv the cross tangent curve associated with space_crv.
    /// \param surf the parametric surface onto which to project the two curves.
    /// \param start_par_pt We may require that the projected curve shall start in a specific
    ///                     point in the parameter domain (input pointer may be NULL).
    /// \param end_par_pt We may require that the projected curve shall end in a specific
    ///                   point in the parameter domain (input pointer may be NULL).
    /// \param epsgeo geometric tolerance for the projection.
    /// \param domain_of_interest if not null, specifies the part of the SplineSurface s
    ///                           parametric domain that will be considered.
    ProjectCurveAndCrossTan(const SplineCurve& space_crv, 
			    const SplineCurve& crosstan_crv,
			    const SplineSurface& surf,
			    const Point* start_par_pt, 
			    const Point* end_par_pt,
			    double epsgeo,
			    const RectDomain* domain_of_interest = NULL);

    /// Destructor.
    virtual ~ProjectCurveAndCrossTan();

    /// Evaluate the curve set.
    /// \param t parameter in which to evaluate.
    /// \return the evaluated points in the input parameter, size of vector is 3.
    virtual std::vector<Go::Point> eval(double t);

    /// Evaluate derivatives in the curve set.
    /// \param t parameter in which to evaluate.
    /// \param n the number of derivatives to evaluate.
    /// \param ders the evaluated points in the input parameter. Size of vector is
    ///             3, each holding 'n + 1' points.
    virtual void eval(double t, int n, std::vector<std::vector<Go::Point> >& ders);

    /// Start parameter of domain.
    /// \return start parameter of domain.
    virtual double start();

    /// End parameter of domain.
    /// \return end parameter of domain.
    virtual double end();

    /// Dimension of space (parameter domain, i.e. 2).
    /// \return geometric dimension of the space.
    // @@sbr Hmm, this seems strange, the curves not living in the same dimension ...
    //       Suppose this means the parameter curve is the most important one.
    virtual int dim();

    /// Whether the approximation is within tolerances in input parameter.
    /// \param par parameter in which to evaluate.
    /// \param approxpos whether the input points are within tolerance from the
    ///                  evaluated points (as given by eval()).
    /// \param tol1 tolerance used to decide approximation accuracy.
    /// \param tol2 tolerance used to decide approximation accuracy.
    /// \return whether the approximation is within tolerances in input parameter.
    virtual bool approximationOK(double par, const std::vector<Go::Point>& approxpos,
				 double tol1, double tol2);

    /// The number of curves in the curve set.
    /// \return the number of curves in the curve set, i.e. 3.
    virtual int nmbCvs();

private:
    const SplineCurve& space_crv_;
    const SplineCurve& crosstan_crv_;
    const SplineSurface& surf_;
    const Point* start_par_pt_; // When projecting end pts may be of special interest.
    const Point* end_par_pt_;
    const double epsgeo_;
    const Go::RectDomain* domain_of_interest_;

    /// Create seed to be used in closest point evaluation.
    /// \param t the parameter for which to make the seed.
    /// \return the seed, size of vector is 2.
    std::vector<double> createSeed(double tpar);

};

///\}
} // namespace Go


#endif //_GOPROJECTCURVEANDTAN_

