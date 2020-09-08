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

    /// Sets the physical volume, needed for the definition
    /// of border optical surfaces
    void SetPhysicalVolume(G4VPhysicalVolume* mother_phys);

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
    G4LogicalVolume* mother_logic_;
    G4VPhysicalVolume* mother_phys_;
    G4double pressure_, temperature_;


  private:
    // Pointers to materials definition
    G4Material* gas_;       ///< Gaseous xenon
    G4Material* aluminum_;  ///< Aluminum
    G4Material* teflon_;    ///< PTFE (Teflon)
    G4Material* tpb_;       /// tpb
    G4Material* quartz_;    /// FusedSilica()


    // Dimensions  

    const G4double elgap_length_; ///< Length of the EL gap
    const G4double elgap_ring_diam_;
    const G4double elgap_ring_thickn_;
    const G4double elgap_ring_height_;
    //   const G4double _wire_diam; ///< diameter of the EL meshes wires
    const G4double GateHV_Mesh_Dist_;
    const G4double anode_length_;
    const G4double anode_diam_;
    const G4double anode_ring_diam_;
    const G4double anode_ring_thickn_;
    const G4double anode_ring_length_;
    const G4double cathode_ring_height_;
    const G4double cathode_ring_diam_;
    const G4double cathode_ring_thickn_;
    const G4double tpb_thickn_;

    const G4double ltube_diam_;
    const G4double ltube_thickn_;
    const G4double ltube_up_length_;
    const G4double ltube_bt_length_;
    const G4double ltube_gap_;
    const G4double ltube_up_gap2cathd_;
    const G4double ltube_bt_gap2cathd_;

    const G4double active_diam_;
    const G4double active_length_;

    const G4double fix_ring_diam_;
    const G4double fix_ring_height_;
    const G4double fix_ring_thickn_;
    
    const G4double fieldcage_length_; ///< Fieldcage length
    const G4double fieldcage_displ_;  ///< Distance btw fieldcage & anode endcap

    const G4double ring_diam_;
    const G4double ring_height_;
    const G4double ring_thickn_;
    const G4double ring_up_bt_gap_;

    const G4double bar_length_;
    const G4double bar_width_;
    const G4double bar_thickn_;
    const G4double bar_addon1_length_;
    const G4double bar_addon1_thickn_;
    const G4double bar_addon2_length_;
    const G4double bar_addon2_thickn_;
    const G4double barBT_length_;
    const G4double barBT_addonBT_length_;
    const G4double barBT_addonBT_thickn_;


    // Dimensions read through input parameters file
    G4double max_step_size_;

    G4bool tpb_coating_;

    // Visibility
    G4bool visibility_, verbosity_;

    G4bool elfield_;


    G4double vessel_length_;


    G4double gate_transparency_;

 /// Position of the fieldcage in the world system of reference
    G4ThreeVector fieldcage_position_;
    /// 
    G4ThreeVector active_position_;
    ///
    G4ThreeVector elgap_position_;

    // G4String _tracking_plane; ///< Tracking plane type: SIPM or PMT       

    // Positions (Z axis) of the TPC electrodes with respect to the
    // FIELDCAGE system of reference
    G4double gate_posz_;    ///< Z position of the gate wrt FIELDCAGE
    G4double anode_posz_;   ///< Z position of the anode wrt FIELDCAGE
    G4double cathode_posz_; ///< Z position of the cathode wrt FIELDCAGE
    G4double AnodezCoord_;

    // Vertex generators
    //HexagonPointSampler* _hexrnd;
    DecagonPointSampler* decrnd_gen_;   // Ruty

    G4double specific_vertex_X_;
    G4double specific_vertex_Y_;
    G4double specific_vertex_Z_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

    G4int el_table_point_id_;
    mutable G4int el_table_index_;
    mutable std::vector<G4ThreeVector> el_table_vertices_;
    G4double el_table_binning_;
    G4double el_table_z_;

  };

} //end namespace nexus
#endif
