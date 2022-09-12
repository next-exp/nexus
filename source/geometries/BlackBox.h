// ----------------------------------------------------------------------------
// nexus | BlackBox.cc
//
// Sapphire disk and DICE board with configurable mask thickness parallelly
// placed in a black box.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef BLACK_BOX_H
#define BLACK_BOX_H

#include "GeometryBase.h"
#include "BlackBoxSiPMBoard.h"

class G4Material;
class G4OpticalSurface;
class G4GenericMessenger;

namespace nexus {

  class BlackBoxSiPMBoard;

  class BlackBox: public GeometryBase
  {
  public:
    /// Constructor
    BlackBox();
    /// Destructor
    ~BlackBox();

    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;


    void Construct();

  private:
    // Dimensions
    G4double world_z_;
    G4double world_xy_;
    G4double box_z_;
    G4double box_xy_;

    BlackBoxSiPMBoard* dice_;
    G4ThreeVector kdb_dimensions_;

    //Messenger for configuration parameters
    G4GenericMessenger* msg_;

    G4bool sapphire_;
    G4bool pedot_coating_;
    G4ThreeVector specific_vertex_;
    G4ThreeVector dice_board_pos_;
    G4ThreeVector sapphire_pos_;
    G4double rotation_;
    G4double mask_thickn_;
    G4double hole_diameter_;
    G4double hole_x_;
    G4double hole_y_;
    G4VPhysicalVolume*  mother_phys_;

  };
  inline void BlackBox::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
    { mother_phys_ = mother_phys; }

} // end namespace nexus

#endif
