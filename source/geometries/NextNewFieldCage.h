// -----------------------------------------------------------------------------
// nexus | NextNewFieldCage.h
//
// Field cage of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXTNEW_FIELDCAGE_H
#define NEXTNEW_FIELDCAGE_H

#include "BaseGeometry.h"
#include <vector>

class G4Material;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4GenericMessenger;
class G4VisAttributes;


namespace nexus {

    class CylinderPointSampler;


  class NextNewFieldCage: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewFieldCage();
    /// Destructor
    ~NextNewFieldCage();

    /// Sets the Logical Volume where Elements will be placed
    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);
    /// Sets the mother's physical volume, needed for the definition
    /// of border optical surfaces
    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);

    /// Generates a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Gives the absolute position of the field cage ensemble
    G4ThreeVector GetPosition() const;

    /// Builder
    void Construct();

  private:
    void DefineMaterials();
    void BuildCathodeGrid();
    void BuildActive();
    void BuildBuffer();
    void BuildELRegion();
    void BuildFieldCage();
    void BuildAnodeGrid();
    void BuildTrackingFrames();


    /// Calculates the vertices for the EL table generation
    void CalculateELTableVertices(G4double radius,
				  G4double binning, G4double z);

  private:

    // Mother Logical Volume
    G4LogicalVolume* mother_logic_;
    G4VPhysicalVolume* mother_phys_;
    G4Material* gas_;
    G4double pressure_;
    G4double temperature_;
    G4double sc_yield_;
    G4double e_lifetime_;

    // Pointers to materials definition
    G4Material* hdpe_;
    G4Material* tpb_;
    G4Material* pedot_;
    G4Material* ito_;
    G4Material* teflon_;
    G4Material* quartz_;
    G4Material* copper_;

    // Dimensions
    G4double dist_feedthroughs_, cathode_thickness_, cathode_gap_;
    G4double windows_end_z_, buffer_length_;
    G4double tube_in_diam_, tube_length_drift_, tube_thickness_, dist_tube_el_; //tube_length_buff_
    G4double hdpe_length_, hdpe_in_diam_, hdpe_out_diam_, hdpe_ledge_;
    G4double  ring_width_, ring_thickness_, tube_z_pos_;
    G4double tpb_thickness_;
    G4double el_gap_z_pos_, pos_z_anode_, pos_z_cathode_, el_gap_length_, grid_thickness_;
    G4double gate_transparency_;
    G4double anode_quartz_thickness_, anode_quartz_diam_; //anode_quartz_transparency_
    G4double cathode_grid_transparency_;
    G4double pedot_thickness_;
    G4double ito_thickness_;
    G4double active_posz_;
    G4double active_length_;

    // Transverse and longitudinal diffusions
    G4double ELtransv_diff_;
    G4double ELlong_diff_;
    G4double drift_transv_diff_;
    G4double drift_long_diff_;

    // Electric field in the EL gap
    G4double ELelectric_field_;

    // Visibility
    G4bool visibility_;

    G4double max_step_size_;
    G4bool elfield_;

    // Vertex generators
    CylinderPointSampler* drift_tube_gen_;
    CylinderPointSampler* hdpe_tube_gen_;
    CylinderPointSampler* xenon_gen_;
    CylinderPointSampler* buffer_gen_;
    CylinderPointSampler* active_gen_;
    CylinderPointSampler* el_gap_gen_;
    CylinderPointSampler* anode_quartz_gen_;
    CylinderPointSampler* cathode_gen_;
    CylinderPointSampler* tracking_frames_gen_;

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

    G4double el_gap_gen_disk_diam_;
    G4double el_gap_gen_disk_x_, el_gap_gen_disk_y_;
    G4double el_gap_gen_disk_zmin_, el_gap_gen_disk_zmax_;

    G4double photoe_prob_;
  };

} //end namespace nexus
#endif
