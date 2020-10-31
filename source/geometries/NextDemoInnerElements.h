// ----------------------------------------------------------------------------
// nexus | NextDemoInnerElements.h
//
// Geometry
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT_DEMO_INNER_ELEMENTS_H
#define NEXT_DEMO_INNER_ELEMENTS_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;


namespace nexus {

  class NextDemoFieldCage;
  class NextDemoEnergyPlane;
  class NextDemoTrackingPlane;

  class NextDemoInnerElements: public BaseGeometry
  {
  public:
    NextDemoInnerElements();
    ~NextDemoInnerElements();
    void Construct() override;
    G4ThreeVector GenerateVertex(const G4String&) const override;
    void SetMotherLogicalVolume(G4LogicalVolume*);
    void SetMotherPhysicalVolume(G4VPhysicalVolume*);

  private:
    G4double gate_sapphire_wdw_distance_;

    G4LogicalVolume* mother_logic_vol_;
    G4VPhysicalVolume* mother_phys_vol_;

    G4Material* gas_;
    G4double pressure_;
    G4double temperature_;

    NextDemoFieldCage*     field_cage_;
    NextDemoTrackingPlane* tracking_plane_;
    NextDemoEnergyPlane*   energy_plane_;

    G4bool verbosity_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;
  };

  inline void NextDemoInnerElements::SetMotherLogicalVolume(G4LogicalVolume* v)
  { mother_logic_vol_ = v; }
  inline void NextDemoInnerElements::SetMotherPhysicalVolume(G4VPhysicalVolume* v)
  { mother_phys_vol_ = v; }

} // end namespace nexus

#endif
