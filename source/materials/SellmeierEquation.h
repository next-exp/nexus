// ----------------------------------------------------------------------------
// nexus | SellmeierEquation.h
//
// The Sellmeier equation is an empirical relationship between refractive
// index and wavelength for a dielectric transparent medium.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SELLMEIER_EQUATION_H
#define SELLMEIER_EQUATION_H


namespace nexus {

  class SellmeierEquation
  {
  public:
    SellmeierEquation(G4double* B, G4double* C);
    ~SellmeierEquation();

    G4double RefractiveIndex(G4double wavelength);

  private:
    G4double B_[3];
    G4double C_[3];
  };

  // Inline definitions ///////////////////////////////////

  inline SellmeierEquation::SellmeierEquation(G4double* B, G4double* C)
  {
    for (unsigned int i=0; i<3; i++) {
      B_[i] = B[i]; C_[i] = C[i];
    }
  }

  inline SellmeierEquation::~SellmeierEquation() {}

  inline G4double SellmeierEquation::RefractiveIndex(G4double wavelength)
  {
    G4double n2 = 1.;
    G4double wl2 = wavelength * wavelength;

    for (unsigned int i=0; i<3; i++)
      n2 += (B_[i] * wl2) / (wl2 - C_[i]);

    return sqrt(n2);
  }

} // end namespace nexus

#endif
