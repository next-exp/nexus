// ----------------------------------------------------------------------------
///  \file   Interpolation.h
///  \brief  General class do linear and bilinear interpolation
///
///  \author  Taylor Contreras <taylorcontreras@g.harvard.edu>
///  \date    16 March 2020
///  \version $Id$
//
///  Copyright (c) 2015 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "G4ThreeVector.hh"

#ifndef _INTERPOLATION_
#define _INTERPOLATION_

namespace nexus {

  inline G4double LinearInterpolation(G4double x, G4double xmin, G4double xmax,
                                      G4double f1, G4double f2)
  {
    G4double result = 0.0;

    if (xmax == xmin){
      G4Exception("[Interpolation]", "LinearInterpolation()", JustWarning,
      "x interval not valid, result is NAN");
      result = NAN;
    }

    else if (x < xmin || x > xmax){
      G4Exception("[Interpolation]", "LinearInterpolation()", JustWarning,
      "x is not in the given interval, result is NAN");
      result = NAN;
    }

    else {
      result = f1 + (f2-f1)*(x-xmin)/(xmax-xmin);
    }

    return result;
  }

  inline G4double BilinearInterpolation(G4double x, G4double xmin, G4double xmax,
                                        G4double y, G4double ymin, G4double ymax,
                                        G4double f11, G4double f12,
                                        G4double f21, G4double f22)
  {
    G4double result = 0.0;

    if ((xmax == xmin) || (ymax == ymin)){
      G4Exception("[Interpolation]", "BilinearInterpolation()", JustWarning,
      "One or more interval is invalid, result is NAN");
      result = NAN;
    }

    else if ((x<xmin) || (x>xmax) || (y<ymin) || (y>ymax)){
      G4Exception("[Interpolation]", "BilinearInterpolation()", JustWarning,
      "x or y is not in the given interval, result is NAN");
      result = NAN;
    }

    else {
      G4double fxmin = (xmax - x)*f11/(xmax - xmin) + (x - xmin)*f21/(xmax - xmin);
      G4double fxmax = (xmax - x)*f12/(xmax - xmin) + (x - xmin)*f22/(xmax - xmin);
      result = (ymax - y)*fxmin/(ymax - ymin) + (y - ymin)*fxmax/(ymax - ymin);
    }

    return result;
  }

}  // end namespace nexus

#endif
