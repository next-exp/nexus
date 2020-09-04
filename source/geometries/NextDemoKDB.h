// ----------------------------------------------------------------------------
///  \file   NextNewKDB.h
///  \brief  Geometry of the Kapton Dice Boards used in the NEW detector.
///
///  \author  <miquel.nebot@ific.uv.es>
///  \date    18 Sept 2013
///  \version $Id$
//
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
//
//  Ruty:: Changing the name from New to Demo
// ----------------------------------------------------------------------------

//#ifndef NEXT_NEW_KDB_H
//#define NEXT_NEW_KDB_H
#ifndef NEXT_DEMO_KDB_H
#define NEXT_DEMO_KDB_H


#include "BaseGeometry.h"
#include <vector>

class G4GenericMessenger;
namespace nexus {class SiPMSensl;}
namespace nexus {class BoxPointSampler;}

namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDB) used in the NEW detector

  class NextDemoKDB: public BaseGeometry
  {
  public:
    /// Constructor
    NextDemoKDB(G4int rows, G4int columns);//, G4double support_thickness);
    /// Destructor
    ~NextDemoKDB();

    /// Builder
    virtual void Construct();
  
    G4ThreeVector GetDimensions() const;
    const std::vector<std::pair<int, G4ThreeVector> >& GetPositions();
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    G4int rows_, columns_;
    //G4double _support_thickness;
    G4ThreeVector dimensions_;
    std::vector<std::pair<int, G4ThreeVector> > positions_;
 
    // Visibility of the shielding
    G4bool visibility_;
    
    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Instance of the SiPM class
    SiPMSensl* sipm_;

    // Vertex generator
    BoxPointSampler* dice_gen_;

    // teflon masks in fron of dices
    G4bool teflon_masks_;
 
  };

} // end namespace nexus

#endif
