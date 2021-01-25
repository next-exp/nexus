// -----------------------------------------------------------------------------
//  nexus | Interpolation.h
//
//  Functions for linear and bilinear interpolation.
//
//  The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <G4ThreeVector.hh>

namespace nexus {

  inline G4double LinearInterpolation(G4double x, G4double xmin, G4double xmax,
                                      G4double f1, G4double f2)
  {
    G4double result = 0.0;

    if (xmax == xmin){
      throw "x interval not valid";
    }

    else if (x < xmin || x > xmax){
      throw "x is not in the given interval";
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
      throw "One or more interval is invalid";
    }

    else if ((x<xmin) || (x>xmax) || (y<ymin) || (y>ymax)){
      throw "x or y is not in the given interval";
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
