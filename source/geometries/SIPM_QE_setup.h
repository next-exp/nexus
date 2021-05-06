// ----------------------------------------------------------------------------
// nexus | SIPM_QE_setup.h
//
// Geometry of a set-up to extract the quantum efficiency of a photomultiplier.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SIPM_QE_SETUP_H
#define SIPM_QE_SETUP_H

#include "BaseGeometry.h"
#include "PmtR11410.h"
#include "Next100SiPM.h"

class G4GenericMessenger;

namespace nexus {

  /// Geometry of a square tracking chamber filled with gaseous xenon

  class SIPM_QE_setup: public BaseGeometry
  {
  public:
    /// Constructor
    SIPM_QE_setup();
    /// Destructor
    ~SIPM_QE_setup();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    /// Define the volumes of the geometry
    void Construct();

  private:
    G4GenericMessenger* msg_;
    G4double z_dist_;
    G4double length_;
    G4double sipm_length_;
    // G4double pmt_length_;

    Next100SiPM sipm_;
    // PmtR11410 pmt_;
  };

} // end namespace nexus

#endif
