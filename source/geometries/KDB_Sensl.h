// -----------------------------------------------------------------------------
// nexus | KDB_Sensl.h
//
// Dice board. It consists of an 8x8 array of
// SensL SiPMs on a kapton board.
//
// Miryam Martínez Vara
// -----------------------------------------------------------------------------

#ifndef KDB_SENSL_H
#define KDB_SENSL_H

#include "BaseGeometry.h"
#include <vector>

class G4GenericMessenger;
class G4LogicalVolume;

namespace nexus {class SiPMSensl;}
namespace nexus {class BoxPointSampler;}

namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDB) used in the NEW detector

  class KDB_Sensl: public BaseGeometry
  {
  public:
    /// Constructor
    KDB_Sensl(G4int rows, G4int columns);//, G4double support_thickness);
    /// Destructor
    ~KDB_Sensl();

    /// Builder
    virtual void Construct();

    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);
    G4ThreeVector GetDimensions() const;
    const std::vector<std::pair<int, G4ThreeVector> >& GetPositions();
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    G4int rows_, columns_;
    //G4double support_thickness_;
    G4ThreeVector dimensions_;
    std::vector<std::pair<int, G4ThreeVector> > positions_;

    // Neighbour gas logical volume
    G4LogicalVolume* mother_logic_;

    // Visibility of the shielding
    G4bool visibility_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Instance of the SiPM class
    SiPMSensl* sipm_;

    // Vertex generator
    BoxPointSampler* dice_gen_;

  };

} // end namespace nexus

#endif
