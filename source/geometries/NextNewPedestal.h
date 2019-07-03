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

#ifndef __NEXTNEW_PEDESTAL__
#define __NEXTNEW_PEDESTAL__

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
    G4LogicalVolume* _mother_logic;
    // Dimensions
    G4double  _table_x, _table_y, _table_z, _y_pos;
   
    // Visibility of the shielding
    G4bool _visibility;

    // Vertex generators
    BoxPointSampler* _table_gen;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} // end namespace nexus

#endif
