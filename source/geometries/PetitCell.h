#ifndef PETITCELL_H
#define PETITCELL_H

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;
namespace nexus {class PetitKDB;}
namespace nexus {class PetitPlainDice;}
namespace nexus {class BoxPointSampler;}

namespace nexus {

  class PetitCell: public BaseGeometry {
  public:
    /// Constructor
    PetitCell();

    /// Destructor
    ~PetitCell();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void Construct();

  private:

    void BuildDetector();
    void BuildLXe();
    void BuildActive();
    void BuildSiPMPlane();

    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* det_logic_;
    G4LogicalVolume* lXe_logic_;

    // Detector dimensions
    //   const G4double vacuum_thickn_; // thickness of vaccuum layer between walls
    //  const G4double outer_wall_thickn_; // Thickness of the outer wall
    const G4double det_in_diam_, det_length_;
    const G4double det_thickness_; // Thickness of the walls of the detector
    const G4double max_step_size_; /// Maximum step size of electrons in Xe

    // ACTIVE gas Xenon
    G4Material* lXe_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Dice class
    PetitKDB* db_;
    PetitPlainDice* pdb_;

    /// Thickness and xy dimension of KDB+SiPM in LXe volume
    G4double db_thickness_, db_lateral_dim_;
    /// Thickness of plain KDB with no SiPMs
    G4double pdb_thickness_;

    G4double active_z_dim_, active_xy_dim_;

    /// Distance of the centre of a kapton dice from the
    /// end of the dice perpendicolar to it.
    G4double dice_offset_;

    BoxPointSampler* active_gen_;

  };

} // end namespace nexus

#endif
