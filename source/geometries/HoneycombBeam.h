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

class G4Box;

namespace nexus {
  class HoneycombBeam: public GeometryBase
  {
  public:
    /// Constructor
    HoneycombBeam(G4double length, G4double height, G4double thickn);

    /// Destructor
    ~HoneycombBeam();

    void Construct();

    G4Box* GetSolidVol() const;
    

  private:

    G4double length_, height_, thickn_;
    G4Box* beam_solid_;
  };

  inline G4Box* HoneycombBeam::GetSolidVol() const {return beam_solid_;}
} // end namespace nexus

#endif
