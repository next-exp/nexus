// -----------------------------------------------------------------------------
// nexus | NextNewInnerElements.h
//
// Inner elements of the NEXT-WHITE detector. They include the field cage,
// the energy plane and the tracking plane.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXTNEW_INNER_ELEMENTS_H
#define NEXTNEW_INNER_ELEMENTS_H

#include "GeometryBase.h"
#include <G4ThreeVector.hh>

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;
namespace nexus { class NextNewEnergyPlane; }
namespace nexus { class NextNewFieldCage; }
namespace nexus { class NextNewTrackingPlane; }

namespace nexus {

  class NextNewInnerElements: public GeometryBase
  {

  public:
    // Constructor
    NextNewInnerElements();
    // Destructor
    ~NextNewInnerElements();

    // Sets the Logical Volume where Inner Elements will be placed
    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);
    //
    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);

    // It Returns the relative position respect to the rest of NEXTNEW geometry
    //G4ThreeVector GetPosition() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


    /// Builder
    void Construct();

  private:

    G4LogicalVolume* mother_logic_;
    G4VPhysicalVolume* mother_phys_;
    G4Material* gas_;
    G4double pressure_;
    G4double temperature_;

    // Detector parts
    NextNewEnergyPlane*   energy_plane_;
    NextNewFieldCage*     field_cage_;
    NextNewTrackingPlane* tracking_plane_;

    // Visibilities
    //G4bool visibility_;

    // Messenger for the definition of control commands
    //G4GenericMessenger* msg_;

  };

} // end namespace nexus

#endif
