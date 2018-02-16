#ifndef PETIT_PLAIN_DICE_H
#define PETIT_PLAIN_DICE_H

#include "BaseGeometry.h"
#include <G4Material.hh>
#include <vector>


class G4GenericMessenger;

namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDB) used in the NEW detector

  class PetitPlainDice: public BaseGeometry
  {
  public:
    /// Constructor
    PetitPlainDice();
    /// Destructor
    ~PetitPlainDice();

    // Dimension setter
    // void SetSize(G4double xsize, G4double ysize);

    //    const std::vector<std::pair<int, G4ThreeVector> >& GetPositions();

    /// Builder
    virtual void Construct();

    void SetMaterial(G4Material& mat);

  private:
    G4int rows_, columns_;

    //    std::vector<std::pair<int, G4ThreeVector> > positions_;

    // Visibility of the shielding
    G4bool visibility_;

    G4double xy_dim_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Outer material
    //G4Material* _out_mat;

 // Reflectivity of teflon
    G4double refl_;

  };

} // end namespace nexus

#endif
