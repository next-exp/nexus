// -----------------------------------------------------------------------------
//  nexus | NextFlexEnergyPlane.h
//
//  NEXT-Flex Energy Plane geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_FLEX_ENERGY_PLANE_H
#define NEXT_FLEX_ENERGY_PLANE_H

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

  class PmtR11410;
  class CylinderPointSampler;

  class NextFlexEnergyPlane: public GeometryBase {

  public:

    NextFlexEnergyPlane();

    virtual ~NextFlexEnergyPlane();

    virtual void Construct();

    virtual G4ThreeVector GenerateVertex(const G4String&) const;

    // Sets as mother volume of all the elements the volume where the class is placed
    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);

    // Sets the neighbouring gas
    void SetNeighGasPhysicalVolume(G4VPhysicalVolume* neigh_gas_phys);

    // Setting the diameter of EP. It is set equal to ACTIVE's.
    void SetDiameter(G4double diam);

    // Setting the origin Z, where the whole Energy Plane starts
    void SetOriginZ(G4double posZ);

    // Returns the final Z of the Tracking Plane (== Final Z of copper plate)
    G4double Get_EP_finZ();

    // Setting the First Energy Plane PMT ID
    void SetFirstSensorID(const G4int first_id);


  private:

    // Read parameters from config file
    void DefineConfigurationParameters();

    // Defines materials needed
    void DefineMaterials();

    // Computes derived dimensions
    void ComputeDimensions();

    // It generates PMT XY positions
    void GeneratePMTpositions();

    // Make the holes (gas $ PMTs) in the solid volume passed.
    G4SubtractionSolid* MakeHoles(G4Tubs* ini_solid);

    // Different builders
    void BuildCopper();
    void BuildTeflon();
    void BuildPMTs();

  private:

    // Logical volume where the class is placed
    G4LogicalVolume* mother_logic_;

    // Physical volume of the neighbouring gas
    G4VPhysicalVolume* neigh_gas_phys_;

    // Verbosity of the geometry
    G4bool verbosity_;

    // Visibilities
    G4bool visibility_;

    // The messenger
    G4GenericMessenger* msg_; // Messenger for configuration parameters

   // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Energy Plane Configuration
    G4bool ep_with_PMTs_;    // PMTs arranged ala NEXT100
    G4bool ep_with_teflon_;  // Teflon mask to reflect light

    // Materials & Components
    G4Material* xenon_gas_;
    G4Material* copper_mat_;
    G4Material* teflon_mat_;
    G4Material* sapphire_mat_;
    G4Material* optical_pad_mat_;

    G4String    wls_matName_;
    G4Material* wls_mat_;

    PmtR11410*  pmt_;

    // Dimensions & Positions
    G4double originZ_;
    G4double diameter_;
    G4double central_hole_diameter_;

    G4double copper_iniZ_;
    G4double copper_finZ_;
    G4double teflon_iniZ_;
    G4double pmt_iniZ_;

    G4double copper_thickness_;
    G4double teflon_thickness_;
    G4double wls_thickness_;

    G4int                      num_pmts_;
    std::vector<G4ThreeVector> pmt_positions_;
    G4double                   pmt_hole_diameter_;

    G4double window_thickness_;
    G4double optical_pad_thickness_;

    // Sensor IDs
    G4int first_sensor_id_;

    // Vertex generators
    CylinderPointSampler* copper_gen_;
    CylinderPointSampler* window_gen_;

  }; // class NextFlexEnergyPlane


  inline void NextFlexEnergyPlane::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
    { mother_logic_ = mother_logic; }

  inline void NextFlexEnergyPlane::SetNeighGasPhysicalVolume(G4VPhysicalVolume* neigh_gas_phys)
    { neigh_gas_phys_ = neigh_gas_phys; }

  inline void NextFlexEnergyPlane::SetOriginZ(G4double posZ)  { originZ_ = posZ;  }

  inline void NextFlexEnergyPlane::SetDiameter(G4double diam) { diameter_ = diam; }

  inline G4double NextFlexEnergyPlane::Get_EP_finZ() { return copper_finZ_; }

  inline void NextFlexEnergyPlane::SetFirstSensorID(const G4int first_id)
    { first_sensor_id_ = first_id; }

} // namespace nexus

#endif


