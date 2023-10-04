// ----------------------------------------------------------------------------
// nexus | HoneycombBeam.h
//
// Beam of the support structure to the EP copper plate.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef HONEYCOMB_BEAM_H
#define HONEYCOMB_BEAM_H

#include "GeometryBase.h"

class G4Trd;

namespace nexus {
  class HoneycombBeam: public GeometryBase
  {
  public:
    /// Constructor
    HoneycombBeam(G4double thickn, G4double length_b,
                  G4double length_t, G4double height);

    /// Destructor
    ~HoneycombBeam();

    void Construct();

    G4Trd* GetSolidVol() const;
    

  private:

    G4double length_b_, length_t_, height_, thickn_;
    G4Trd* beam_solid_;
  };

  inline G4Trd* HoneycombBeam::GetSolidVol() const {return beam_solid_;}
} // end namespace nexus

#endif
