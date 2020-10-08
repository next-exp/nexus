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

class G4GenericMessenger;
class G4LogicalVolume;

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

    BaseGeometry* tile_;

    private:
    void Construct();
    void BuildBox();
    void BuildSensors();

    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* LXe_logic_;

    G4bool visibility_;
    G4double reflectivity_;

    G4double source_pos_x_, source_pos_y_, source_pos_z_;

    G4String tile_type_;
    G4double box_size_, box_thickness_;

    G4double ih_x_size_, ih_y_size_, ih_z_size_;
    G4double ih_thick_wall_, ih_thick_roof_;
    G4double source_tube_thick_wall_, source_tube_int_radius_;
    G4double dist_source_roof_, source_tube_thick_roof_;

    G4double n_tile_rows_, n_tile_columns_;
    G4double tile_thickn_, full_row_size_, full_col_size_;

    G4double dist_ihat_panel_, panel_thickness_;
    G4double active_depth_;
    G4double entry_panel_width_, dist_entry_panel_lat_panel_;

    G4double lat_panel_len_, lat_panel_width_;
    G4double low_lat_panel_width_, low_lat_panel_height_, dist_lat_panels_;

    G4double x_pos_;

    G4double max_step_size_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };
}
#endif
