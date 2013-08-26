// ----------------------------------------------------------------------------
///  \file   SellmeierEquation.h
///  \brief  Calculation of refractive index using Sellmeier's equation
///  
///  \author  <justo.martin-albo@ific.uv.es>
///
///  \date    7 Nov 2011
///  \version $Id$
///
///  Copyright (c) 2011-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef SELLMEIER_EQUATION_H
#define SELLMEIER_EQUATION_H


namespace nexus {

  /// The Sellmeier equation is an empirical relationship between refractive
  /// index and wavelength for a dielectric transparent medium.

  class SellmeierEquation
  {
  public:
    SellmeierEquation(G4double* B, G4double* C);
    ~SellmeierEquation();

    G4double RefractiveIndex(G4double wavelength);

  private:
    G4double _B[3];
    G4double _C[3];
  };

  // Inline definitions ///////////////////////////////////

  inline SellmeierEquation::SellmeierEquation(G4double* B, G4double* C)
  { 
    for (unsigned int i=0; i<3; i++) {
      _B[i] = B[i]; _C[i] = C[i];
    }
  }

  inline SellmeierEquation::~SellmeierEquation() {}

  inline G4double SellmeierEquation::RefractiveIndex(G4double wavelength)
  {
    G4double n2 = 1.;
    G4double wl2 = wavelength * wavelength;
    
    for (unsigned int i=0; i<3; i++)
      n2 += (_B[i] * wl2) / (wl2 - _C[i]);
    
    return sqrt(n2);
  }

} // end namespace nexus

#endif
