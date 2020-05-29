// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <miquel.nebot@ific.uv.es>,
///  \date     29 Nov 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef _NEXTNEW_PEDESTAL___
#define _NEXTNEW_PEDESTAL___

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

#include <G4Navigator.hh>

class G4GenericMessenger;

namespace nexus {

  class NextNewPedestal: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewPedestal();

    /// Destructor
    ~NextNewPedestal();

    /// Sets the logical volume where the pedestal will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();

    /// Setters and getters
    void SetPosition(G4double pos);
    G4ThreeVector GetDimensions() const;

  private:
    // Mother Logical Volume of the ICS
    G4LogicalVolume* mother_logic_;
    // Dimensions
    G4double  table_x_, table_y_, table_z_, y_pos_;
   
    // Visibility of the shielding
    G4bool visibility_;

    // Vertex generators
    BoxPointSampler* table_gen_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

  };

} // end namespace nexus

#endif
