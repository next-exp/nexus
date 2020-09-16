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
#include "DecagonPointSampler.h"

class G4GenericMessenger;

namespace nexus {

  class CylinderPointSampler2020;

  class NextDemoFieldCage: public BaseGeometry
  {

  public:
    /// Constructor
    NextDemoFieldCage();
    /// Destructor
    ~NextDemoFieldCage();

    /// Sets as mother the volume where the class is placed
    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);

    /// Sets the physical volume, needed for the definition
    /// of border optical surfaces
    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);

    void DefineMaterials();
    void BuildActive();
    void BuildCathodeGrid();
    void BuildBuffer();
    void BuildELRegion();
    void BuildFieldCage();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();

  private:
    /// Calculates the vertices for the EL table generation
    void CalculateELTableVertices(G4double radius,
				  G4double binning, G4double z);

    G4Navigator* geom_navigator_;

    // Logical volume where the class is placed
    G4LogicalVolume* mother_logic_;
    G4VPhysicalVolume* mother_phys_;
    G4double pressure_, temperature_;


    // Pointers to materials definition
    G4Material* gas_;       ///< Gaseous xenon
    G4Material* aluminum_;  ///< Aluminum
    G4Material* teflon_;    ///< PTFE (Teflon)
    G4Material* tpb_;       /// tpb
    G4Material* quartz_;    /// FusedSilica()


    // Dimensions

    const G4double gate_cathode_centre_dist_;
    const G4double grid_thickn_, gate_transparency_;
    const G4double buffer_length_;
    const G4double cath_grid_transparency_;
    const G4double cathode_ring_diam_;
    const G4double el_gap_length_, elgap_ring_diam_;
    const G4double light_tube_drift_start_z_;
    const G4double light_tube_drift_end_z_;
    const G4double ltube_thickn_;
    const G4double light_tube_buff_start_z_;
    const G4double light_tube_buff_end_z_;
    const G4double anode_length_;
    const G4double anode_diam_;
    const G4double tpb_thickn_;
    const G4double active_diam_;
    const G4double ring_diam_;
    const G4double ring_height_;
    const G4double ring_thickn_;
    const G4double dist_gate_first_ring_;
    const G4double bar_start_z_, bar_end_z_;
    const G4double bar_width_;
    const G4double bar_thickn_;
    const G4double bar_addon1_length_;
    const G4double bar_addon1_thickn_;
    const G4double bar_addon2_length_;
    const G4double bar_addon2_thickn_;

    // Visibility and verbosity
    G4bool visibility_, verbosity_;

    G4double max_step_size_;

    G4double active_length_, active_zpos_;
    G4double cathode_grid_zpos_, el_gap_zpos_;
    G4double ltube_drift_length_, ltube_buff_length_;
    G4double bar_length_;
    G4double drift_transv_diff_, drift_long_diff_;
    G4double ELtransv_diff_, ELlong_diff_;
    G4bool elfield_, ELelectric_field_;

    // Vertex generators
    CylinderPointSampler2020* active_gen_;


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


    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

} //end namespace nexus
#endif
