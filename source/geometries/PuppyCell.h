#ifndef PUPPYCELL_H
#define PUPPYCELL_H

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;
namespace nexus {class PuppyKDB;}
namespace nexus {class PuppyPlainDice;}
namespace nexus {class BoxPointSampler;}

namespace nexus {

  class PuppyCell: public BaseGeometry {
  public:
    /// Constructor
    PuppyCell();

    /// Destructor
    ~PuppyCell();

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
    PuppyKDB* db_;
    PuppyPlainDice* pdb_;

    G4double db_z_;

    G4double active_z_dim_, active_xy_dim_;

  };

} // end namespace nexus

#endif
