// ----------------------------------------------------------------------------
// nexus | Next100FieldCage.h
//
// Geometry of the NEXT-100 field cage.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_FIELDCAGE_H
#define NEXT100_FIELDCAGE_H

#include "BaseGeometry.h"
#include <vector>

class G4Material;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4GenericMessenger;
class G4Navigator;

namespace nexus {

  class CylinderPointSampler2020;


  class Next100FieldCage: public BaseGeometry
  {
  public:
    Next100FieldCage();
    ~Next100FieldCage();
    void Construct() override;
    G4ThreeVector GenerateVertex(const G4String& region) const override;

    G4ThreeVector GetActivePosition() const;
    G4double GetDistanceGateSapphireWindows() const;

    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);
    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);

  private:
    void DefineMaterials();
    void BuildActive();
    void BuildCathodeGrid();
    void BuildBuffer();
    void BuildELRegion();
    void BuildFieldCage();

    void CalculateELTableVertices(G4double, G4double, G4double);

    // Dimensions
    const G4double active_diam_;
    const G4double gate_cathode_centre_dist_, gate_sapphire_wdw_dist_;
    const G4double cathode_diam_, grid_thickn_, cathode_gap_;
    const G4double teflon_total_length_, teflon_thickn_;
    const G4double gate_teflon_dist_;
    const G4int n_panels_;
    const G4double tpb_thickn_, el_gap_diam_, el_gap_length_;
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

    // Variables for the EL table generation
    G4double el_table_binning_; ///< Binning of EL lookup table
    G4int el_table_point_id_; ///< Id of the EL point to be simulated
    mutable G4int el_table_index_; ///< Index for EL lookup table generation
    mutable std::vector<G4ThreeVector> table_vertices_;

    // Visibility of the geometry
    G4bool visibility_;
    // Verbosity of the geometry
    G4bool verbosity_;

    G4double active_length_, active_zpos_, buffer_length_;
    G4double el_gap_zpos_, cathode_grid_zpos_;


    // Vertex generators
    CylinderPointSampler2020* active_gen_;
    CylinderPointSampler2020* buffer_gen_;
    CylinderPointSampler2020* teflon_gen_;
    CylinderPointSampler2020* xenon_gen_;
    CylinderPointSampler2020* el_gap_gen_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Logical volume where the class is placed
    G4LogicalVolume* mother_logic_;
    G4VPhysicalVolume* mother_phys_;
    G4Material* gas_;
    G4double pressure_;
    G4double temperature_;

    // Pointers to materials definition
    G4Material* hdpe_;
    G4Material* tpb_;
    G4Material* teflon_;
    G4Material* copper_;

    G4double el_gap_gen_disk_diam_;
    G4double el_gap_gen_disk_x_, el_gap_gen_disk_y_;
    G4double el_gap_gen_disk_zmin_, el_gap_gen_disk_zmax_;
  };

} //end namespace nexus
#endif
