// -----------------------------------------------------------------------------
//  nexus | NextFlexTrackingPlane.h
//
//  * Info:         : NEXT-Flex Tracking Plane geometry for performance studies.
//  * Author        : <jmunoz@ific.uv.es>
//  * Creation date : January 2020
// -----------------------------------------------------------------------------

#ifndef NEXT_FLEX_TRACKING_PLANE_H
#define NEXT_FLEX_TRACKING_PLANE_H

#include <G4ThreeVector.hh>
#include <vector>

#include "BaseGeometry.h"


class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;
class G4Tubs;
class G4SubtractionSolid;
class G4Navigator;


namespace nexus {

  class CylinderPointSampler2020;
  class GenericPhotosensor;


  class NextFlexTrackingPlane: public BaseGeometry {

  public:

    NextFlexTrackingPlane();

    virtual ~NextFlexTrackingPlane();

    virtual void Construct();
    
    virtual G4ThreeVector GenerateVertex(const G4String&) const;

    // Sets as mother volume of all the elements the volume where the class is placed
    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);

    // Sets the neighbouring gas
    void SetNeighGasPhysicalVolume(G4VPhysicalVolume* neigh_gas_phys);

    // Setting the diameter of EP. It is set equal to ACTIVE's.
    void SetDiameter(G4double diam);

    // Setting the origin Z, where the whole Energy Plane starts (to the left == z<0)
    void SetOriginZ(G4double posZ);

    // Returns the initial Z of the Energy Plane (== Initial Z of copper plate)
    G4double Get_TP_iniZ();

    // Setting the First Tracking Plane SiPM ID
    void SetFirstSensorID(const G4int first_id);


  private:

    // Read parameters from config file
    void DefineConfigurationParameters();

    // Defines materials needed
    void DefineMaterials();

    // Computes derived dimensions
    void ComputeDimensions();

    // It generates SiPM XY positions
    void GenerateSiPMpositions();

    // Different builders
    void BuildCopper();
    void BuildTeflon();
    G4LogicalVolume* BuildSiPM();

  private:

    // Logical volume where the class is placed
    G4LogicalVolume* _mother_logic;

    // Physical volume of the neighbouring gas
    G4VPhysicalVolume* _neigh_gas_phys;

    // Verbosity of the geometry
    G4bool _verbosity;

    // Visibilities
    G4bool _visibility;

    // The messenger
    G4GenericMessenger* _msg; // Messenger for configuration parameters

   // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Materials & Components
    G4Material* _xenon_gas;
    G4Material* _copper_mat;
    G4Material* _teflon_mat;

    GenericPhotosensor* _SiPM;

    G4String    _wls_matName;
    G4Material* _wls_mat;

    // Dimensions & Positions
    G4double _originZ;
    G4double _diameter;
    G4double _SiPM_ANODE_dist;

    G4double _SiPM_sizeX;
    G4double _SiPM_sizeY;
    //G4double _SiPM_thickness;
    G4double _SiPM_case_thickness;
    G4double _SiPM_pitchX;
    G4double _SiPM_pitchY;
    G4double _SiPM_binning;
    G4double _num_SiPMs;

    std::vector<G4ThreeVector> _SiPM_positions;

    G4double _copper_thickness;
    G4double _copper_iniZ;

    G4double _teflon_thickness;
    G4double _teflon_hole_diam;
    G4double _teflon_iniZ;
    
    G4double _wls_thickness;

    // Sensor IDs
    G4int _first_sensor_id;

    // Vertex generators
    CylinderPointSampler2020* _copper_gen;

  }; // class NextFlexTrackingPlane


  inline void NextFlexTrackingPlane::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
    { _mother_logic = mother_logic; }

  inline void NextFlexTrackingPlane::SetNeighGasPhysicalVolume(G4VPhysicalVolume* neigh_gas_phys)
    { _neigh_gas_phys = neigh_gas_phys; }

  inline void NextFlexTrackingPlane::SetOriginZ(G4double posZ)  { _originZ = posZ;  }

  inline void NextFlexTrackingPlane::SetDiameter(G4double diam) { _diameter = diam; }

  inline G4double NextFlexTrackingPlane::Get_TP_iniZ() { return _copper_iniZ; }

  inline void NextFlexTrackingPlane::SetFirstSensorID(const G4int first_id)
    { _first_sensor_id = first_id; }

} // namespace nexus

#endif


