// -----------------------------------------------------------------------------
//  nexus | NextFlexFieldCage.h
//
//  * Info:         : NEXT-Flex Field Cage geometry for performance studies.
//  * Author        : <jmunoz@ific.uv.es>
//  * Creation date : January 2020
// -----------------------------------------------------------------------------

#ifndef NEXT_FLEX_FIELD_CAGE_H
#define NEXT_FLEX_FIELD_CAGE_H

#include <G4ThreeVector.hh>
#include "BaseGeometry.h"

class G4LogicalVolume;
class G4Material;
class G4GenericMessenger;


namespace nexus {

  class CylinderPointSampler2020;

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


  private:

    // Read parameters from config file
    void DefineConfigurationParameters();

    // Defines materials needed
    void DefineMaterials();

    // Computes derived dimensions
    //void ComputeDimensions();

    // Different builders
    void BuildActive();
    void BuildCathode();
    void BuildBuffer();
    void BuildELgap();


  private:

    // Logical volume where the class is placed
    G4LogicalVolume* _mother_logic;

    // Verbosity of the geometry
    G4bool _verbosity;

    // Visibilities
    G4bool _visibility;

    // The messenger
    G4GenericMessenger* _msg; // Messenger for configuration parameters

    // Materials
    G4double    _gas_pressure, _gas_temperature;
    G4Material* _xenon_gas;

    // ACTIVE
    G4double _active_diam,       _active_length;
    G4double _drift_transv_diff, _drift_long_diff;

    // CATHODE
    G4double _cathode_thickn, _cathode_transparency;

    // BUFFER
    G4double _buffer_length;
    G4double _buffer_finalZ;

    // EL_GAP
    G4double _el_gap_length;
    G4bool   _el_field_on;
    G4double _el_field_int;
    G4double _el_transv_diff, _el_long_diff;
    G4double _anode_thickn, _anode_transparency;
    G4double _gate_thickn,  _gate_transparency;

    // LIGHT_TUBE


    // Vertex generators
    CylinderPointSampler2020* _active_gen;
    CylinderPointSampler2020* _buffer_gen;
    CylinderPointSampler2020* _el_gap_gen;


  }; // class NextFlexFieldCage

  inline void NextFlexFieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
    { _mother_logic = mother_logic; }

  inline G4double NextFlexFieldCage::Get_ACTIVE_diam()   { return _active_diam;   }

  inline G4double NextFlexFieldCage::Get_BUFFER_finalZ() { return _buffer_finalZ; }


} // namespace nexus

#endif


