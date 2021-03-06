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
#ifndef _GOPARAMCURVE_H
#define _GOPARAMCURVE_H

#include "Point.h"
#include "GeomObject.h"
#include "CompositeBox.h"
#include "DirectionCone.h"
#include "Values.h"
#include <vector>
#include <boost/smart_ptr.hpp>


namespace Go
{
///\addtogroup geometry
///\{


class SplineCurve;

    /** Base class for parametric curves in Go
     *
     */

class ParamCurve : public GeomObject
{
public:
    /// virtual destructor - ensures safe inheritance
    virtual ~ParamCurve();

    /// Evaluate the curve's position at a given parameter
    /// \param pt the evaluated position will be written to this Point
    /// \param tpar the parameter for which we wish to evaluate the curve
    virtual void point(Point& pt, double tpar) const = 0;

    /// Evaluate the curve's position and a certain number of derivatives
    /// at a given parameter.
    /// \param pts the evaluated position and derivatives (tangent, curvature vector, etc.)
    ///            will be written to this vector.  The first entry will be the position,
    ///            the second entry will be the first derivative, etc.  The size of this
    ///            vector must be set to 'derivs'+ 1 prior to calling this function.
    /// \param tpar the parameter for which we want to evaluate the curve
    /// \param derivs the number of derivatives we want to have calculated
    /// \param from_right specify whether we should calculate derivatives 'from the right'
    ///                   or 'from the left' (default is from the right).  This matters 
    ///                   only when the curve presents discontinuities in its derivatives.
    virtual void point(std::vector<Point>& pts, 
		       double tpar,
		       int derivs,
		       bool from_right = true) const = 0;


    /// Evaluate the curve's position at a certain parameter
    /// \param tpar the parameter for which we want to evaluate the curve's position
    /// \return the curve's position for the parameter 'tpar'.
    /// NB: This function is implemented in terms of the ParamCurve's virtual
    /// 'point(...)' functions, but is itself not virtual.  If you make a concrete 
    /// subclass and wish to make this function visible to the user, you must
    /// put a 'using ParamCurve::point' statement in the class definition.
    Point point(double tpar) const;

    /// Evaluate the curve's position and a certain number of derivatives 
    /// at a given parameter.
    /// \param tpar the parameter for which we want to evaluate the curve
    /// \param derivs the number of derivatives we want to have calculated
    /// \param from_right specify whether we should calculate derivatives 'from the right'
    ///                   or 'from the left' (default is from the right).  This matters 
    ///                   only when the curve presents discontinuities in its derivatives.
    /// \return a STL vector containing the evaluated position and the specified number
    ///         of derivatives.  The first entry will be the position, the second entry
    ///         will be the first derivative, etc.  The total size of the returned vector
    ///         will be 'derivs' + 1.
    /// NB: This function is implemented in terms of the ParamCurve's virtual
    /// 'point(...)' functions, but is itself not virtual.  If you make a concrete 
    /// subclass and wish to make this function visible to the user, you must
    /// put a 'using ParamCurve::point' in the class definition.
    std::vector<Point> point(double tpar, int derivs, bool from_right = true) const;

    /// Query the start parameter of the curve
    /// \return the curve's start parameter
    virtual double startparam() const = 0;

    /// Query the end parameter of the curve
    /// \return the curve's end parameter
    virtual double endparam() const = 0;

    /// Set the parameter direction of the curve.  The curve's parameter interval will 
    /// always remain constant, but by flipping the parameter direction, the curve will
    /// be traced the opposite way when moving a parameter over the parameter interval.
    /// \param switchparam if true, and the curve is 2D, the x and y
    /// coordinates should be swapped. This is used when turning the
    /// orientation of bounded (trimmed) surfaces.
    virtual void reverseParameterDirection(bool switchparam = false) = 0;
    
    /// If the definition of this ParamCurve contains a SplineCurve describing its 
    /// spatial shape, then this function will return a pointer to this SplineCurve.
    /// Otherwise it will return a null pointer.
    /// \return a pointer to the internal SplineCurve, if it exists.  Null pointer 
    ///         otherwise.
    virtual SplineCurve* geometryCurve() = 0;

    /// Query whether the curve is degenerate (collapsed into a single point).
    /// \param degenerate_epsilon the tolerance used in determine whether the curve
    ///        is degenerate.  A curve is considered degenerate if its total length 
    ///        is shorter than this value.
    /// \return 'true' if the curve is degenerate, 'false' otherwise.
    virtual bool isDegenerate(double degenerate_epsilon) = 0;

    /// Returns a curve which is a part of this curve.  The result is NEWed, so the 
    /// user is responsible for deleting it.  NB: This function only work for
    /// \em derived classes of ParamCurve, namely those who are spline-based.
    /// \param from_par start value of parameter interval that will define the subcurve
    /// \param to_par end value of parameter interval that will define the subcurve
    /// \param fuzzy since subCurve works on those curves who are spline-based, this
    ///              tolerance defines how close the start and end parameter must be
    ///              to an existing knot in order to be considered \em on the knot.
    /// \return a pointer to a new subcurve which represents the part of the curve
    ///         between 'from_par' and 'to_par'.  It will be spline-based and have a 
    ///         k-regular knotvector.  The user is responsible for deleting this subcurve
    ///         when it is no longer needed.
    virtual ParamCurve* subCurve(double from_par, double to_par,
				 double fuzzy = DEFAULT_PARAMETER_EPSILON) const = 0;

    /// The clone-function is herited from GeomObject, but overridden here to get a 
    /// covariant return type (for those compilers that allow this).
// #ifdef _MSC_VER
// #if _MSC_VER < 1300
//     virtual GeomObject* clone() const = 0;
// #else
//     virtual ParamCurve* clone() const = 0;
// #endif //_MSC_VER < 1300
// #else
    virtual ParamCurve* clone() const = 0;
// #endif

    /// Creates a DirectionCone which covers all tangent directions of this
    /// curve.
    /// \return the smallest DirectionCone containing all tangent directions of 
    ///         this curve.
    virtual DirectionCone directionCone() const = 0;
 
    /// Creates a composite box enclosing the curve. The composite box
    /// consists of an inner and an edge box. The inner box is
    /// supposed to be made from the interior of the curve, while the
    /// edge box is made from the start and end points. The default
    /// implementation simply makes both boxes identical to the
    /// regular bounding box.
    /// \return the CompositeBox enclosing the curve.
    virtual CompositeBox compositeBox() const;

    /// append a curve to this curve, with eventual reparametrization
    /// NB: This virtual member function currently only works for SplineCurves 
    /// and CurveOnSurfaces.  Moreover, 'this' curve and the 'cv' curve must 
    /// be of the same type.
    /// \param cv the curve to append to 'this' curve.
    /// \param reparam specify whether or not there should be reparametrization
    virtual void appendCurve(ParamCurve* cv, bool reparam=true) = 0;

    /// append a curve to this curve, with eventual reparmetrization
    /// \param cv the curve to append to 'this' curve.
    /// \param continuity the required continuity at the transition.  Can be G^(-1) 
    ///        and upwards.
    /// \param dist a measure of the local distorsion around the transition in order
    ///             to achieve the specified continuity.
    /// \param reparam specify whether or not there should be reparametrization
    virtual void appendCurve(ParamCurve* cv,
			     int continuity, double& dist, bool reparam=true) = 0;

    /// Estimate the length of the curve, by sampling it at a certain number of points
    /// and calculating the linear approximation to the curve through these points.
    /// \param numpts number of sample points used
    /// \return the estimated curve length
    double estimatedCurveLength(int numpts = 4) const;

    /// Estimate the length of an interval of the curve, by sampling it at a certain 
    /// number of points in the interval and calculating the linear approximation 
    /// through these points.
    /// \param tmin parameter at start of interval
    /// \param tmax parameter at end of interval
    /// \param numpts number of sample points used
    /// \return the estimated curve length
    double estimatedCurveLength(double tmin, double tmax, int numpts = 4) const;

    /// Compute the closest point from an interval of this curve to a specified point.
    /// \param pt point we want to find the closest point to
    /// \param tmin start parameter of search interval
    /// \param tmax end parameter of search interval
    /// \param clo_t upon function return, 'clo_t' will contain the parameter value of 
    ///              the closest point found.
    /// \param clo_pt upon function return, 'clo_pt' will contain the position of the
    ///               closest point found.
    /// \param clo_dist upon function return, 'clo_dist' will containn the distance 
    ///                 between 'pt' and the closest point found.
    /// \param seed pointer to initial guess value, provided by the user (can be 0,
    ///             for which the algorithm will determine a (hopefully) reasonable
    ///             choice).
    virtual void closestPoint(const Point& pt,
			      double tmin,
			      double tmax,
			      double& clo_t,
			      Point& clo_pt,
			      double& clo_dist,
			      double const *seed = 0) const = 0;

    /// Compute the closest point from this curve to a specified point, taking the whole
    /// curve into account (not just an interval of it).
    /// \param pt point we want to find the closest point to
    /// \param clo_t upon function return, 'clo_t' will contain the parameter value of 
    ///              the closest point found.
    /// \param clo_pt upon function return, 'clo_pt' will contain the position of the
    ///               closest point found.
    /// \param clo_dist upon function return, 'clo_dist' will containn the distance 
    ///                 between 'pt' and the closest point found.
    void closestPoint(const Point& pt, double& clo_t, Point& clo_pt, double& clo_dist) const;

    /// If the ParamCurve is divided up into logical segments, this function will return 
    /// the parameter value of the "next segment", starting from a parameter given by the user.
    /// If no division into logical segments exist, then it is the start- or end parameter that
    /// is returned.
    /// \param par the parameter from which we start the search for the next segment.
    /// \param forward whether to search forwards or backwards along the parameter domain.
    /// \param tol the tolerance to determine or not 'par' is already located on the start of 
    ///            the next segment.
    /// \return the parameter value of the next segment.
    virtual double nextSegmentVal(double par, bool forward, double tol) const;

protected:
    void closestPointGeneric(const Point&   pt,
			     double    tmin,
			     double    tmax,
			     double guess_param,
			     double&   clo_t,
			     Point&  clo_pt,
			     double&   clo_dist) const;
};

///\}
} // namespace Go



#endif // _GOPARAMCURVE_H


