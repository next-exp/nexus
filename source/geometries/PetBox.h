// ----------------------------------------------------------------------------
// nexus | PetBox.cc
//
// This class implements the geometry of a symmetric box of LXe.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PET_BOX_H
#define PET_BOX_H

#include "BaseGeometry.h"
#include "TileBaseGeometry.h"

class G4GenericMessenger;
class G4LogicalVolume;
class G4VPhysicalVolume;

namespace nexus {
  class TileHamamatsuVUV;
  class TileHamamatsuBlue;
  class TileFBK;
}

namespace nexus {
  class PetBox : public BaseGeometry {

  public:
    // Constructor
    PetBox();
    //Destructor
    ~PetBox();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    TileBaseGeometry* tile_;

    private:
    void Construct();
    void BuildBox();
    void BuildSensors();

    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* LXe_logic_;

    G4bool visibility_;
    G4double reflectivity_;
    G4bool tile_vis_;
    G4double tile_refl_;
    G4double sipm_pde_;

    G4double source_pos_x_, source_pos_y_, source_pos_z_;

    G4String tile_type_;
    G4double time_binning_;

    G4double box_size_, box_thickness_;

    G4double ih_x_size_, ih_y_size_, ih_z_size_;
    G4double ih_thick_wall_, ih_thick_roof_;
    G4double source_tube_thick_wall_, source_tube_int_radius_;
    G4double dist_source_roof_, source_tube_thick_roof_;

    G4double n_tile_rows_, n_tile_columns_;
    G4double tile_thickn_, full_row_size_, full_col_size_;

    G4double dist_lat_panels_;

    G4double dist_ihat_entry_panel_, panel_thickness_;
    G4double entry_panel_x_size_, entry_panel_y_size_;
    G4double dist_entry_panel_ground_;

    G4double dist_entry_panel_horiz_panel_, dist_entry_panel_vert_panel_;

    G4double lat_panel_len_;
    G4double horiz_lat_panel_z_size_, horiz_lat_panel_y_pos_;
    G4double vert_lat_panel_z_size_;

    G4double dist_dice_flange_;
    G4double panel_sipm_xy_size_, dist_sipms_panel_sipms_;
    G4double wls_depth_;

    G4double max_step_size_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

}
#endif
