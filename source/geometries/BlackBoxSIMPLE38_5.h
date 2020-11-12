// ----------------------------------------------------------------------------
///  \file   BlackBoxSIMPLE38_5.h
///  \brief  ...
///
///  \author   Miryam Martínez Vara <Miryam.Martinez@ific.uv.es>    
///  \date     2 Oct 2020
///  \version  $Id$
///
///  Copyright (c) 2020 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef BLACK_BOX_SIMPLE_38_5_H
#define BLACK_BOX_SIMPLE_38_5_H

#include "BaseGeometry.h"
#include "KDB_Sensl.h"

class G4Material;
class G4OpticalSurface;
class G4GenericMessenger;
namespace nexus { class SpherePointSampler; } //PUEDE QUE QUITE ESTO


namespace nexus {
  
  /// Spherical chamber filled with xenon (liquid or gas)
  
  class BlackBoxSIMPLE38_5: public BaseGeometry
  {
  public:
    /// Constructor
    BlackBoxSIMPLE38_5();
    /// Destructor
    ~BlackBoxSIMPLE38_5();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;


    void Construct();

  private:
    G4OpticalSurface* GetPhotOptSurf();

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
    G4double dice_board_z_pos_;

    G4bool _visibility;

    //Messenger for configuration parameters
    G4GenericMessenger* _msg;

    /// Specific vertex for AD_HOC region
    G4double _specific_vertex_X;
    G4double _specific_vertex_Y;
    G4double _specific_vertex_Z;

  };

} // end namespace nexus

#endif
