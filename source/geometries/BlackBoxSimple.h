// ----------------------------------------------------------------------------
//  nexus | BlackBoxSimple.h
//  SensL KDB (no mask) in a black box.
//  Author:  Miryam Martínez Vara
// ----------------------------------------------------------------------------

#ifndef BLACK_BOX_SIMPLE_H
#define BLACK_BOX_SIMPLE_H

#include "BaseGeometry.h"
#include "KDB_Sensl.h"

class G4Material;
class G4OpticalSurface;
class G4GenericMessenger;

namespace nexus {

  class BlackBoxSimple: public BaseGeometry
  {
  public:
    /// Constructor
    BlackBoxSimple();
    /// Destructor
    ~BlackBoxSimple();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;


    void Construct();

  private:
    // Dimensions
    G4double _world_z;
    G4double _world_xy;
    // SiPMs per Dice Board
    const G4int SiPM_rows_, SiPM_columns_;
    // Dice board
    KDB_Sensl* dice_;
    G4ThreeVector kdb_dimensions_;
    G4double dice_board_x_pos_;
    G4double dice_board_y_pos_;

    G4bool _visibility;

    //Messenger for configuration parameters
    G4GenericMessenger* _msg;

    /// Specific vertex for AD_HOC region
    G4double _specific_vertex_X;
    G4double _specific_vertex_Y;
    G4double _specific_vertex_Z;
    G4double _dice_board_z_pos;
    G4double _rotation;

  };

} // end namespace nexus

#endif