// -----------------------------------------------------------------------------
//  nexus | NextFlexFieldCage.h
//
//  NEXT-Flex Field Cage geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_FLEX_FIELD_CAGE_H
#define NEXT_FLEX_FIELD_CAGE_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

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
    G4LogicalVolume* mother_logic_;

    // Physical volumes
    G4VPhysicalVolume* active_phys_;
    G4VPhysicalVolume* buffer_phys_;

    // Verbosity of the geometry
    G4bool verbosity_;

    // Visibilities
    G4bool visibility_;
    G4bool fiber_sensor_visibility_;

    // The messenger
    G4GenericMessenger* msg_; // Messenger for configuration parameters

    // Energy Plane Configuration
    G4bool fc_with_fibers_;

    // ACTIVE
    G4double active_diam_,       active_length_;
    G4double drift_transv_diff_, drift_long_diff_;

    // CATHODE
    G4double cathode_thickness_, cathode_transparency_;

    // BUFFER
    G4double buffer_length_;
    G4double buffer_finalZ_;

    // EL_GAP
    G4double el_gap_length_;
    G4bool   el_field_on_;
    G4double el_field_int_;
    G4double el_transv_diff_, el_long_diff_;
    G4double anode_thickness_, anode_transparency_;
    G4double gate_thickness_,  gate_transparency_;
    G4double photoe_prob_;

    // LIGHT_TUBE
    G4double light_tube_thickness_;
    G4double fc_length_;
    G4double light_tube_inner_rad_;
    G4double light_tube_outer_rad_;
    G4double wls_thickness_;

    // FIBERS
    G4double fiber_thickness_;
    G4int    fiber_claddings_;
    G4double fiber_extra_length_;
    G4double fiber_inner_rad_;
    G4double fiber_light_tube_gap_;
    G4double cladding_perc_;
    G4double fiber_iniZ_;
    G4double fiber_finZ_;
    G4int    num_fibers_;

    // FIBER SENSORS
    GenericPhotosensor* left_sensor_;
    GenericPhotosensor* right_sensor_;

    G4double fiber_sensor_size_;
    G4double fiber_sensor_thickness_;
    G4double fiber_sensor_binning_;
    G4int    num_fiber_sensors_;


    // Materials
    G4Material* xenon_gas_;
    G4double    gas_pressure_, gas_temperature_;
    G4double    gas_e_lifetime_, gas_sc_yield_;

    G4Material* teflon_mat_;
    G4String    wls_mat_name_;
    G4Material* wls_mat_;
    G4Material* cathode_mat_;
    G4Material* gate_mat_;
    G4Material* anode_mat_;
    G4String    fiber_mat_name_;
    G4Material* fiber_mat_;
    G4Material* iClad_mat_;
    G4Material* oClad_mat_;

    // Sensor IDs
    G4int first_left_sensor_id_;
    G4int first_right_sensor_id_;


    // Vertex generators
    CylinderPointSampler2020* active_gen_;
    CylinderPointSampler2020* buffer_gen_;
    CylinderPointSampler2020* el_gap_gen_;
    CylinderPointSampler2020* light_tube_gen_;
    CylinderPointSampler2020* fiber_gen_;


    // Parameters related with EL_GAP vertex generator
    G4double el_gap_gen_disk_diam_;
    G4double el_gap_gen_disk_x_, el_gap_gen_disk_y_;
    G4double el_gap_gen_disk_zmin_, el_gap_gen_disk_zmax_;

  }; // class NextFlexFieldCage


  inline void NextFlexFieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
    { mother_logic_ = mother_logic; }

  inline G4VPhysicalVolume* NextFlexFieldCage::Get_BUFFER_phys()
    { return buffer_phys_; }

  inline G4double NextFlexFieldCage::Get_ACTIVE_diam()   { return active_diam_; }

  inline G4double NextFlexFieldCage::Get_FC_outer_rad()  { return light_tube_outer_rad_; }

  inline G4double NextFlexFieldCage::Get_BUFFER_finalZ() { return buffer_finalZ_; }

  inline G4double NextFlexFieldCage::Get_EL_GAP_iniZ()   { return - el_gap_length_; }

  inline void NextFlexFieldCage::SetFirstLeftSensorID(const G4int id)
    { first_left_sensor_id_ = id; }

  inline void NextFlexFieldCage::SetFirstRightSensorID(const G4int id)
    { first_right_sensor_id_ = id; }

} // namespace nexus

#endif


