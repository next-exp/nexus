// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <paola.ferrario@ific.uv.es>
///  \date     1 Mar 2012
///  \version  $Id$
///
///  Copyright (c) 2012 NEXT Collaboration
//
//  Updated to NextDemo++  by  Ruth Weiss Babai <ruty.wb@gmail.com> 
//  Based on: NextNewFieldCage.h and Next1EL.h
//  Date:   June 2019
// ----------------------------------------------------------------------------

#ifndef __NEXT_DEMO_FIELDCAGE_H
#define __NEXT_DEMO_FIELDCAGE_H

#include <vector>
#include <G4LogicalVolume.hh>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

#include "BaseGeometry.h"
//#include "CylinderPointSampler.h"
//#include "HexagonPointSampler.h"
#include "DecagonPointSampler.h"

//class G4Material;
//class G4LogicalVolume;
class G4GenericMessenger;
//class G4VisAttributes;


namespace nexus {

  /// This is a geometry formed by the reflector polygon and 
  /// TPB layer if needed
  /// Geometry of the NEXT1-EL detector
  //class HexagonPointSampler;
  //class DecagonPointSampler;
  

  class NextDemoFieldCage: public BaseGeometry
  {

  public:
    /// Constructor
    NextDemoFieldCage(const G4double vessel_length);
    //NextDemoFieldCage();
    /// Destructor
    ~NextDemoFieldCage();

    /// Sets as mother the volume where the class is placed
    // void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);
    /// Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Sets the mother's physical volume, needed for the definition
    /// of border optical surfaces
    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    ///
    G4ThreeVector GetPosition() const;

    G4double GetAnodezCoord() const;


    /// Builder
    void Construct();

  private:
    /// Calculates the vertices for the EL table generation
    void CalculateELTableVertices(G4double radius, 
				  G4double binning, G4double z);


  private:
    // Logical volume where the class is placed
    G4LogicalVolume* _mother_logic;
    G4VPhysicalVolume* _mother_phys;
    G4double _pressure, _temperature;


  private:
    // Pointers to materials definition
    G4Material* _gas;       ///< Gaseous xenon
    G4Material* _aluminum;  ///< Aluminum
    G4Material* _teflon;    ///< PTFE (Teflon)
    G4Material* _tpb;       /// tpb
    G4Material* _quartz;    /// FusedSilica()


    // Dimensions  

    const G4double _elgap_length; ///< Length of the EL gap
    const G4double _elgap_ring_diam;
    const G4double _elgap_ring_thickn;
    const G4double _elgap_ring_height;
    //   const G4double _wire_diam; ///< diameter of the EL meshes wires
    const G4double _GateHV_Mesh_Dist;
    const G4double _anode_length;
    const G4double _anode_diam;
    const G4double _anode_ring_diam;
    const G4double _anode_ring_thickn;
    const G4double _anode_ring_length;
    const G4double _cathode_ring_height;
    const G4double _cathode_ring_diam;
    const G4double _cathode_ring_thickn;
    const G4double _tpb_thickn;

    const G4double _ltube_diam;
    const G4double _ltube_thickn;
    const G4double _ltube_up_length;
    const G4double _ltube_bt_length;
    const G4double _ltube_gap;
    const G4double _ltube_up_gap2cathd;
    const G4double _ltube_bt_gap2cathd;

    const G4double _active_diam;
    const G4double _active_length;

    const G4double _fix_ring_diam;
    const G4double _fix_ring_height;
    const G4double _fix_ring_thickn;
    
    const G4double _fieldcage_length; ///< Fieldcage length
    const G4double _fieldcage_displ;  ///< Distance btw fieldcage & anode endcap

    const G4double _ring_diam;
    const G4double _ring_height;
    const G4double _ring_thickn;
    const G4double _ring_up_bt_gap;

    const G4double _bar_length;
    const G4double _bar_width;
    const G4double _bar_thickn;
    const G4double _bar_addon1_length;
    const G4double _bar_addon1_thickn;
    const G4double _bar_addon2_length;
    const G4double _bar_addon2_thickn;
    const G4double _barBT_length;
    const G4double _barBT_addonBT_length;
    const G4double _barBT_addonBT_thickn;


    // Dimensions read through input parameters file
    G4double _max_step_size;

    G4bool _tpb_coating;

    // Visibility
    G4bool _visibility, _verbosity;

    G4bool _elfield;


    G4double _vessel_length;


    G4double _gate_transparency;

 /// Position of the fieldcage in the world system of reference
    G4ThreeVector _fieldcage_position;
    /// 
    G4ThreeVector _active_position;
    ///
    G4ThreeVector _elgap_position;

    // G4String _tracking_plane; ///< Tracking plane type: SIPM or PMT       

    // Positions (Z axis) of the TPC electrodes with respect to the
    // FIELDCAGE system of reference
    G4double _gate_posz;    ///< Z position of the gate wrt FIELDCAGE
    G4double _anode_posz;   ///< Z position of the anode wrt FIELDCAGE
    G4double _cathode_posz; ///< Z position of the cathode wrt FIELDCAGE
    G4double _AnodezCoord;

    // Vertex generators
    //HexagonPointSampler* _hexrnd;
    DecagonPointSampler* _decrnd_gen;   // Ruty

    G4double _specific_vertex_X;
    G4double _specific_vertex_Y;
    G4double _specific_vertex_Z;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    G4int _el_table_point_id;
    mutable G4int _el_table_index;
    mutable std::vector<G4ThreeVector> _el_table_vertices;
    G4double _el_table_binning;
    G4double _el_table_z;

  };

} //end namespace nexus
#endif
