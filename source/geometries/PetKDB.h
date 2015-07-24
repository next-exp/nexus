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
    PetKDB();

    /// Destructor
    ~PetKDB();

    // Dimension setter
    void SetXYsize(G4double xysize);

    G4ThreeVector GetDimensions() const;
    const std::vector<std::pair<int, G4ThreeVector> >& GetPositions();

    /// Builder
    virtual void Construct();

    void SetMaterial(G4Material& mat);

  private:
    G4int rows_, columns_;
    //G4double _support_thickness;
    G4ThreeVector dimensions_;
    std::vector<std::pair<int, G4ThreeVector> > positions_;
 
    // Visibility of the shielding
    G4bool visibility_;

    // xy dimensions
    G4double xysize_;
    
    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;    
    
    G4double sipm_pitch_;
    
    // Outer material
    //G4Material* _out_mat;


 
  };

} // end namespace nexus

#endif
