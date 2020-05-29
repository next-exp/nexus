// ----------------------------------------------------------------------------
// nexus | PMT_QE_setup.h
//
// Geometry of a set-up to extract the quantum efficiency of a photomultiplier.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PMT_QE_SETUP
#define PMT_QE_SETUP

#include "BaseGeometry.h"
#include "PmtR7378A.h"
#include "PmtR11410.h"

class G4GenericMessenger;

namespace nexus {

  /// Geometry of a square tracking chamber filled with gaseous xenon

  class PMT_QE_setup: public BaseGeometry
  {
  public:
    /// Constructor
    PMT_QE_setup();
    /// Destructor
    ~PMT_QE_setup();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    /// Define the volumes of the geometry
    void Construct();

  private:
    G4GenericMessenger* msg_;
    G4double z_dist_;
    G4double length_;
    G4double pmt_length_;

    PmtR11410 pmt_;
    //PmtR7378A pmt_;
  };

} // end namespace nexus

#endif
