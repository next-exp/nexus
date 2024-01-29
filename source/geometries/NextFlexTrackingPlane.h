// -----------------------------------------------------------------------------
//  nexus | NextFlexTrackingPlane.h
//
//  NEXT-Flex Tracking Plane geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_FLEX_TRACKING_PLANE_H
#define NEXT_FLEX_TRACKING_PLANE_H

#include "GeometryBase.h"
#include <G4ThreeVector.hh>
#include <vector>

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;
class G4Tubs;
class G4SubtractionSolid;
class G4Navigator;


namespace nexus {

  class CylinderPointSampler;
  class GenericPhotosensor;


  class NextFlexTrackingPlane: public GeometryBase {

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
    void BuildSiPMs();

  private:

    // Logical volume where the class is placed
    G4LogicalVolume* mother_logic_;

    // Physical volume of the neighbouring gas
    G4VPhysicalVolume* neigh_gas_phys_;

    // Verbosities of the geometry
    G4bool verbosity_;
    G4bool sipm_verbosity_;

    // Visibilities
    G4bool visibility_;
    G4bool SiPM_visibility_;

    // The messenger
    G4GenericMessenger* msg_; // Messenger for configuration parameters

   // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Materials & Components
    G4Material* xenon_gas_;
    G4Material* copper_mat_;
    G4Material* teflon_mat_;

    GenericPhotosensor* SiPM_;

    G4String    wls_mat_name_;
    G4Material* wls_mat_;

    // Dimensions & Positions
    G4double origin_z_;
    G4double diameter_;
    G4double kapton_anode_dist_;

    G4double SiPM_size_x_;
    G4double SiPM_size_y_;
    G4double SiPM_size_z_;
    G4double SiPM_pitch_x_;
    G4double SiPM_pitch_y_;
    G4double SiPM_binning_;
    G4double num_SiPMs_;

    std::vector<G4ThreeVector> SiPM_positions_;

    G4double copper_thickness_;
    G4double copper_iniZ_;

    G4double teflon_thickness_;
    G4double teflon_hole_diam_;
    G4double teflon_iniZ_;

    G4double wls_thickness_;

    // Sensor IDs
    G4int first_sensor_id_;

    // Vertex generators
    CylinderPointSampler* copper_gen_;

  }; // class NextFlexTrackingPlane


  inline void NextFlexTrackingPlane::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
    { mother_logic_ = mother_logic; }

  inline void NextFlexTrackingPlane::SetNeighGasPhysicalVolume(G4VPhysicalVolume* neigh_gas_phys)
    { neigh_gas_phys_ = neigh_gas_phys; }

  inline void NextFlexTrackingPlane::SetOriginZ(G4double posZ)  { origin_z_ = posZ;  }

  inline void NextFlexTrackingPlane::SetDiameter(G4double diam) { diameter_ = diam; }

  inline G4double NextFlexTrackingPlane::Get_TP_iniZ() { return copper_iniZ_; }

  inline void NextFlexTrackingPlane::SetFirstSensorID(const G4int first_id)
    { first_sensor_id_ = first_id; }

} // namespace nexus

#endif


