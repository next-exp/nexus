// ----------------------------------------------------------------------------
///  \file   PetPlainDice.h
///  \brief  Geometry of the 
///
///  \author  <miquel.nebot@ific.uv.es>
///  \date    18 Sept 2013
///  \version $Id$
//
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef PET_PLAIN_DICE_H
#define PET_PLAIN_DICE_H

#include "BaseGeometry.h"
#include <G4Material.hh>
#include <vector>


class G4GenericMessenger;

namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDB) used in the NEW detector

  class PetPlainDice: public BaseGeometry
  {
  public:
    /// Constructor
    PetPlainDice();
    /// Destructor
    ~PetPlainDice();

    // Dimension setter
    void SetSize(G4double xsize, G4double ysize);

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

    // y size
    G4double ysize_;
    
    // x size
    G4double xsize_;
    
    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;    
    
    // Outer material
    //G4Material* _out_mat;

 // Reflectivity of teflon
G4double refl_;
 
  };

} // end namespace nexus

#endif
