// -----------------------------------------------------------------------------
// nexus | NextNewKDB.h
//
// Dice board of the NEXT-WHITE detector. It consists of an 8x8 array of
// SensL SiPMs on a kapton board.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_NEW_KDB_H
#define NEXT_NEW_KDB_H

#include "GeometryBase.h"
#include <vector>

class G4GenericMessenger;
class G4LogicalVolume;

namespace nexus {class SiPMSensl;}
namespace nexus {class BoxPointSamplerLegacy;}

namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDB) used in the NEW detector

  class NextNewKDB: public GeometryBase
  {
  public:
    /// Constructor
    NextNewKDB(G4int rows, G4int columns);//, G4double support_thickness);
    /// Destructor
    ~NextNewKDB();

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

    //TP SiPMs time binning
    G4double time_binning_;

    // Visibility 
    G4bool visibility_, sipm_visibility_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Instance of the SiPM class
    SiPMSensl* sipm_;

    // Vertex generator
    BoxPointSamplerLegacy* dice_gen_;

  };

} // end namespace nexus

#endif
