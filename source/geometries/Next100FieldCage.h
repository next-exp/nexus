// ----------------------------------------------------------------------------
// nexus | Next100FieldCage.h
//
// Geometry of the NEXT-100 field cage.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_FIELDCAGE_H
#define NEXT100_FIELDCAGE_H

#include "GeometryBase.h"
#include <vector>

class G4Material;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4GenericMessenger;
class G4Navigator;

namespace nexus {

  class CylinderPointSampler;


  class Next100FieldCage: public GeometryBase
  {
  public:
    Next100FieldCage(G4double grid_thickn);
    ~Next100FieldCage();
    void Construct() override;
    G4ThreeVector GenerateVertex(const G4String& region) const override;

    G4ThreeVector GetActivePosition() const;

    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);
    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);
    void SetELtoSapphireWDWdistance(G4double);

  private:
    void DefineMaterials();
    void BuildActive();
    void BuildCathode();
    void BuildBuffer();
    void BuildELRegion();
    void BuildLightTube();
    void BuildFieldCage();

    // Dimensions
    G4double gate_sapphire_wdw_dist_;
    const G4double active_diam_, n_panels_, active_ext_radius_;
    const G4double grid_thickn_;
    const G4double cathode_int_diam_, cathode_ext_diam_, cathode_thickn_, cathode_mesh_diam_;
    const G4double teflon_long_length_, teflon_buffer_length_;
    const G4double teflon_thickn_;
    const G4double el_gap_length_, el_mesh_diam_, el_mesh_rot_;
    const G4double gate_ext_diam_, gate_int_diam_, gate_ring_thickn_;
    const G4double gate_teflon_dist_, gate_cathode_dist_;
    const G4double hdpe_tube_int_diam_, hdpe_tube_ext_diam_, hdpe_length_;
    const G4double ring_ext_diam_, ring_int_diam_, ring_thickn_;
    const G4double drift_ring_dist_, buffer_ring_dist_;
    const G4double buffer_first_ring_dist_, ring_drift_buffer_dist_;
    const G4int num_drift_rings_, num_buffer_rings_;
    const G4double tpb_thickn_;
    const G4double overlap_;

    // Diffusion constants
    G4double drift_transv_diff_, drift_long_diff_;
    G4double ELtransv_diff_; ///< transversal diffusion in the EL gap
    G4double ELlong_diff_; ///< longitudinal diffusion in the EL gap
    // Electric field
    G4bool elfield_;
    G4double ELelectric_field_; ///< electric field in the EL region
    // Transparencies of grids
    const G4double cath_grid_transparency_, el_grid_transparency_;

    //Step size
    G4double max_step_size_;

    // Visibility of the geometry
    G4bool visibility_;
    G4bool grid_visibility_;
    // Verbosity of the geometry
    G4bool verbosity_;

    // Use fake mesh
    G4bool use_dielectric_grid_;

    // Parameters related to look-up table generation
    G4double el_gap_gen_disk_diam_;
    G4double el_gap_gen_disk_x_, el_gap_gen_disk_y_;
    G4double el_gap_gen_disk_zmin_, el_gap_gen_disk_zmax_;

    G4double active_length_, buffer_length_;
    G4double teflon_drift_length_, teflon_drift_zpos_,teflon_buffer_zpos_;
    G4double active_zpos_, cathode_zpos_, gate_zpos_, el_gap_zpos_;
    G4double gate_grid_zpos_, anode_grid_zpos_;
    G4double active_gen_radius_;


    // Vertex generators
    CylinderPointSampler* active_gen_;
    CylinderPointSampler* buffer_gen_;
    CylinderPointSampler* teflon_gen_;
    CylinderPointSampler* xenon_gen_;
    CylinderPointSampler* el_gap_gen_;
    CylinderPointSampler* hdpe_gen_;
    CylinderPointSampler* ring_gen_;
    CylinderPointSampler* cathode_gen_;
    CylinderPointSampler* gate_gen_;
    CylinderPointSampler* anode_gen_;
    CylinderPointSampler* holder_gen_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Logical volume where the class is placed
    G4LogicalVolume* mother_logic_;
    G4VPhysicalVolume* mother_phys_;
    G4VPhysicalVolume* active_phys_;
    G4VPhysicalVolume* buffer_phys_;
    G4Material* gas_;
    G4double pressure_;
    G4double temperature_;
    G4double sc_yield_;
    G4double e_lifetime_;
    G4double photoe_prob_;

    // Pointers to materials definition
    G4Material* hdpe_;
    G4Material* tpb_;
    G4Material* teflon_;
    G4Material* copper_;
    G4Material* steel_;
  };


  inline void Next100FieldCage::SetELtoSapphireWDWdistance(G4double distance){
    gate_sapphire_wdw_dist_ = distance;
  }

} //end namespace nexus
#endif
