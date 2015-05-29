// ----------------------------------------------------------------------------
///  \file   PetKDB.h
///  \brief  Geometry of the Kapton Dice Boards used in the NEW detector.
///
///  \author  <miquel.nebot@ific.uv.es>
///  \date    18 Sept 2013
///  \version $Id$
//
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef PET_KDB_H
#define PET_KDB_H

#include "BaseGeometry.h"
#include <G4Material.hh>
#include <vector>


class G4GenericMessenger;

namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDB) used in the NEW detector

  class PetKDB: public BaseGeometry
  {
  public:
    /// Constructor
    PetKDB(G4int rows, G4int columns);//, G4double support_thickness);
    /// Destructor
    ~PetKDB();

    G4ThreeVector GetDimensions() const;
    const std::vector<std::pair<int, G4ThreeVector> >& GetPositions();

    /// Builder
    virtual void Construct();

    void SetMaterial(G4Material& mat);

  private:
    G4int _rows, _columns;
    //G4double _support_thickness;
    G4ThreeVector _dimensions;
    std::vector<std::pair<int, G4ThreeVector> > _positions;
 
    // Visibility of the shielding
    G4bool _visibility;
    
    // Messenger for the definition of control commands
    G4GenericMessenger* _msg;    
    
    // Outer material
    //G4Material* _out_mat;
 
  };

} // end namespace nexus

#endif
