// -----------------------------------------------------------------------------
//  nexus | NextFlexFieldCage.h
//
//  NEXT-Flex Field Cage geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_FLEX_FIELD_CAGE_H
#define NEXT_FLEX_FIELD_CAGE_H

#include <G4ThreeVector.hh>
#include "BaseGeometry.h"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;


namespace nexus {

  class CylinderPointSampler2020;
  class GenericPhotosensor;


  class NextFlexFieldCage: public BaseGeometry {

  public:

    NextFlexFieldCage();

    virtual ~NextFlexFieldCage();

    virtual void Construct();
    
    virtual G4ThreeVector GenerateVertex(const G4String&) const;

    // Sets as mother volume of all the elements the volume where the class is placed
    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);

    // Returns the ACTIVE diameter
    G4double Get_ACTIVE_diam();

    // Returns the posZ where the BUFFER ends
    G4double Get_BUFFER_finalZ();

    // Returns the posZ where the EL_GAP starts
    G4double Get_EL_GAP_iniZ();

    // Returns the outer radius of the Field Cage (= Light Tube outer radius)
    G4double Get_FC_outer_rad();

    // Returns the BUFFER physical volume
    G4VPhysicalVolume* Get_BUFFER_phys();

    // Setting the First Left Sensor ID (of the fibers barrel).
    void SetFirstLeftSensorID(const G4int first_id);

    // Setting the First Right Sensor ID (of the fibers barrel).
    void SetFirstRightSensorID(const G4int first_id);
    

  private:

    // Read parameters from config file
    void DefineConfigurationParameters();

    // Defines materials needed
    void DefineMaterials();

    // Computes derived dimensions
    void ComputeDimensions();

    // Different builders
    void BuildActive();
    void BuildCathode();
    void BuildBuffer();
    void BuildELgap();
    void BuildLightTube();
    void BuildFibers();
    void BuildFiberSensors();

  private:

    // Logical volume where the class is placed
    G4LogicalVolume* _mother_logic;

    // Physical volumes
    G4VPhysicalVolume* _active_phys;
    G4VPhysicalVolume* _buffer_phys;

    // Verbosity of the geometry
    G4bool _verbosity;

    // Visibilities
    G4bool _visibility;

    // The messenger
    G4GenericMessenger* _msg; // Messenger for configuration parameters

    // Energy Plane Configuration
    G4bool _fc_with_fibers;

    // ACTIVE
    G4double _active_diam,       _active_length;
    G4double _drift_transv_diff, _drift_long_diff;

    // CATHODE
    G4double _cathode_thickness, _cathode_transparency;

    // BUFFER
    G4double _buffer_length;
    G4double _buffer_finalZ;

    // EL_GAP
    G4double _el_gap_length;
    G4bool   _el_field_on;
    G4double _el_field_int;
    G4double _el_transv_diff, _el_long_diff;
    G4double _anode_thickness, _anode_transparency;
    G4double _gate_thickness,  _gate_transparency;

    // LIGHT_TUBE
    G4double _light_tube_thickness;
    G4double _fc_length;
    G4double _light_tube_inner_rad;
    G4double _light_tube_outer_rad;
    G4double _wls_thickness;

    // FIBERS
    G4double _fiber_thickness;
    G4int    _fiber_claddings;
    G4double _fiber_extra_length;
    G4double _fiber_inner_rad;
    G4double _fiber_light_tube_gap;
    G4double _cladding_perc;
    G4double _fiber_iniZ;
    G4double _fiber_finZ;
    G4int    _num_fibers;

    // FIBER SENSORS
    GenericPhotosensor* _left_sensor;
    GenericPhotosensor* _right_sensor;

    G4double _fiber_sensor_size;
    G4double _fiber_sensor_thickness;
    G4double _fiber_sensor_binning;
    G4int    _num_fiber_sensors;


    // Materials
    G4double    _gas_pressure, _gas_temperature;
    G4Material* _xenon_gas;

    G4Material* _teflon_mat;
    G4String    _wls_matName;
    G4Material* _wls_mat;
    G4String    _fiber_matName;
    G4Material* _fiber_mat;
    G4Material* _iClad_mat;
    G4Material* _oClad_mat;
    G4Material* _fiber_sensor_case_mat;
    G4Material* _fiber_sensor_mat;

    // Sensor IDs
    G4int _first_left_sensor_id;
    G4int _first_right_sensor_id;


    // Vertex generators
    CylinderPointSampler2020* _active_gen;
    CylinderPointSampler2020* _buffer_gen;
    CylinderPointSampler2020* _el_gap_gen;
    CylinderPointSampler2020* _light_tube_gen;
    CylinderPointSampler2020* _fiber_gen;

  }; // class NextFlexFieldCage


  inline void NextFlexFieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
    { _mother_logic = mother_logic; }

  inline G4VPhysicalVolume* NextFlexFieldCage::Get_BUFFER_phys()
    { return _buffer_phys; }

  inline G4double NextFlexFieldCage::Get_ACTIVE_diam()   { return _active_diam; }

  inline G4double NextFlexFieldCage::Get_FC_outer_rad()  { return _light_tube_outer_rad; }

  inline G4double NextFlexFieldCage::Get_BUFFER_finalZ() { return _buffer_finalZ; }

  inline G4double NextFlexFieldCage::Get_EL_GAP_iniZ()   { return - _el_gap_length; }

  inline void NextFlexFieldCage::SetFirstLeftSensorID(const G4int first_id)
    { _first_left_sensor_id = first_id; }

  inline void NextFlexFieldCage::SetFirstRightSensorID(const G4int first_id)
    { _first_right_sensor_id = first_id; }

} // namespace nexus

#endif


