// -----------------------------------------------------------------------------
//  nexus | NextFlexEnergyPlane.h
//
//  NEXT-Flex Energy Plane geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_FLEX_ENERGY_PLANE_H
#define NEXT_FLEX_ENERGY_PLANE_H

#include <G4ThreeVector.hh>
#include <vector>

#include "BaseGeometry.h"
#include "PmtR11410.h"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;
class G4Tubs;
class G4SubtractionSolid;
class G4Navigator;


namespace nexus {

  class CylinderPointSampler2020;

  class NextFlexEnergyPlane: public BaseGeometry {

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

    // Make the PMT corresponding holes in the solid volume passed.
    G4SubtractionSolid* MakePMTholes(G4SubtractionSolid* the_solid, G4double length);

    // Different builders
    void BuildCopper();
    void BuildTeflon();
    void BuildPMTs();

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

    // Energy Plane Configuration
    G4bool _ep_with_PMTs;    // PMTs arranged ala NEXT100
    G4bool _ep_with_teflon;  // Teflon mask to reflect light

    // Materials & Components
    G4Material* _xenon_gas;
    G4Material* _copper_mat;
    G4Material* _teflon_mat;
    G4Material* _sapphire_mat;
    G4Material* _optical_pad_mat;

    G4String    _wls_matName;
    G4Material* _wls_mat;

    // Dimensions & Positions
    G4double _originZ;
    G4double _diameter;
    G4double _central_hole_diameter;

    G4double _copper_iniZ;
    G4double _copper_finZ;
    G4double _teflon_iniZ;

    G4double _copper_thickness;
    G4double _teflon_thickness;
    G4double _wls_thickness;

    PmtR11410*                 _pmt;
    G4int                      _num_pmts;
    std::vector<G4ThreeVector> _pmt_positions;
    G4double                   _pmt_hole_diameter;
    G4double                   _pmt_hole_posZ;

    G4double _window_thickness;
    G4double _optical_pad_thickness;

    // Sensor IDs
    G4int _first_sensor_id;

    // Vertex generators
    CylinderPointSampler2020* _copper_gen;
    CylinderPointSampler2020* _window_gen;

  }; // class NextFlexEnergyPlane


  inline void NextFlexEnergyPlane::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
    { _mother_logic = mother_logic; }

  inline void NextFlexEnergyPlane::SetNeighGasPhysicalVolume(G4VPhysicalVolume* neigh_gas_phys)
    { _neigh_gas_phys = neigh_gas_phys; }

  inline void NextFlexEnergyPlane::SetOriginZ(G4double posZ)  { _originZ = posZ;  }

  inline void NextFlexEnergyPlane::SetDiameter(G4double diam) { _diameter = diam; }

  inline G4double NextFlexEnergyPlane::Get_EP_finZ() { return _copper_finZ; }

  inline void NextFlexEnergyPlane::SetFirstSensorID(const G4int first_id)
    { _first_sensor_id = first_id; }

} // namespace nexus

#endif


