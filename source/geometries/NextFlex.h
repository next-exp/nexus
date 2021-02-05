// -----------------------------------------------------------------------------
//  nexus | NextFlex.h
//
//  NEXT-Flex Detector geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_FLEX_H
#define NEXT_FLEX_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4LogicalVolume;
class G4Material;
class G4GenericMessenger;


namespace nexus {

  class NextFlexFieldCage;
  class NextFlexEnergyPlane;
  class NextFlexTrackingPlane;
  class CylinderPointSampler2020;

  // This is a geometry placer that encloses:
  // The FIELD_CAGE     (Including ACTIVE, BUFFER, EL_GAP & LIGHT_TUBE)
  // The ENERGY_PLANE   (Considering all the variety it may consist of)
  // The TRACKING_PLANE (Considering all the variety it may consist of)

  class NextFlex : public BaseGeometry
  {

  public:
    // Constructor
    NextFlex();

    // Destructor
    ~NextFlex();

    // Builder
    void Construct();

    // Generate vertices within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


  private:

    // Read parameters from config file
    void DefineConfigurationParameters();

    // Defines materials needed
    void DefineMaterials();

    // Different builders
    void BuildICS(G4LogicalVolume* mother_logic);

  private:

    const G4int FIRST_ENERGY_SENSOR_ID      =      0;
    const G4int FIRST_TRACKING_SENSOR_ID    =   1000;
    const G4int FIRST_LEFT_FIBER_SENSOR_ID  = 100000;
    const G4int FIRST_RIGHT_FIBER_SENSOR_ID = 200000;

    // Verbosity of the geometry
    G4bool verbosity_;

    // Visibility of the geometry
    G4bool ics_visibility_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Materials
    G4String    gas_name_;
    G4double    gas_pressure_, gas_temperature_;
    G4double    sc_yield_;
    G4double    e_lifetime_;
    G4Material* xenon_gas_;
    G4Material* copper_mat_;
    G4Material* air_mat_;

    // Dimensions
    G4double lightTube_ICS_gap_;
    G4double ics_thickness_;

    // Detector parts
    NextFlexFieldCage*     field_cage_;
    NextFlexEnergyPlane*   energy_plane_;
    NextFlexTrackingPlane* tracking_plane_;

    // Vertex generators
    CylinderPointSampler2020* copper_gen_;

    // AD-HOC vertex
    G4double adhoc_x_, adhoc_y_, adhoc_z_;
  };

} // end namespace nexus

#endif
