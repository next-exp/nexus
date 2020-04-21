// ----------------------------------------------------------------------------
///  \file   NextNewKDB.h
///  \brief  Geometry of the Kapton Dice Boards used in the NEW detector.
///
///  \author  <miquel.nebot@ific.uv.es>
///  \date    18 Sept 2013
///  \version $Id$
//
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef NEXT_NEW_KDB_H
#define NEXT_NEW_KDB_H

#include "BaseGeometry.h"
#include <vector>

class G4GenericMessenger;
class G4LogicalVolume;

namespace nexus {class SiPMSensl;}
namespace nexus {class BoxPointSampler;}

namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDB) used in the NEW detector

  class NextNewKDB: public BaseGeometry
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
    G4int _rows, _columns;
    //G4double _support_thickness;
    G4ThreeVector _dimensions;
    std::vector<std::pair<int, G4ThreeVector> > _positions;

    // Neighbour gas logical volume
    G4LogicalVolume* _mother_logic;
 
    // Visibility of the shielding
    G4bool _visibility;
    
    // Messenger for the definition of control commands
    G4GenericMessenger* _msg;

    // Instance of the SiPM class
    SiPMSensl* _sipm;

    // Vertex generator
    BoxPointSampler* _dice_gen;

    // teflon masks in fron of dices
    G4bool _teflon_masks;
 
  };

} // end namespace nexus

#endif
