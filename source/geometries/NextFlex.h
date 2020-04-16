// -----------------------------------------------------------------------------
//  nexus | NextFlex.h
//
//  * Info          : NEXT-Flex Detector geometry for performance studies.
//  * Author        : <jmunoz@ific.uv.es>
//  * Creation date : January 2020
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

    const G4int FIRST_ENERGY_SENSOR_ID      =     0;
    const G4int FIRST_TRACKING_SENSOR_ID    =  1000;
    const G4int FIRST_LEFT_FIBER_SENSOR_ID  = 10000;
    const G4int FIRST_RIGHT_FIBER_SENSOR_ID = 20000;

    // Verbosity of the geometry
    G4bool _verbosity;

    // Visibility of the geometry
    G4bool _ics_visibility;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg;

    // Materials
    G4String    _gas_name;
    G4double    _gas_pressure, _gas_temperature;
    G4double    _sc_yield;
    G4double    _e_lifetime;
    G4Material* _xenon_gas;
    G4Material* _copper_mat;

    // Dimensions
    G4double _lightTube_ICS_gap;
    G4double _ics_thickness;

    // Detector parts
    NextFlexFieldCage*     _field_cage;
    NextFlexEnergyPlane*   _energy_plane;
    NextFlexTrackingPlane* _tracking_plane;

    // Vertex generators
    CylinderPointSampler2020* _copper_gen;

    // AD-HOC vertex
    G4double _adhoc_x, _adhoc_y, _adhoc_z;
  };

} // end namespace nexus

#endif
