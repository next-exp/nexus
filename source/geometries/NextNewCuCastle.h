// ----------------------------------------------------------------------------
///  \file   
///  \brief    Copper castle inside the shielding lead 
///  \author   <miquel.nebot@ific.uv.es>,
///  \date     3 Feb 2014
///  \version  $Id$
///
///  Copyright (c) 2014 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_CU_CASTLE__
#define __NEXTNEW_CU_CASTLE__

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

class G4GenericMessenger;

namespace nexus {

  class NextNewCuCastle: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewCuCastle();

    /// Destructor
    ~NextNewCuCastle();

    /// Sets the Logical Volume where Radon tube will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:
    // Mother Logical Volume of the ICS
    G4LogicalVolume* _mother_logic;
    // Dimensions
    G4double  _x, _y, _z, _thickness;
   
    // Visibility of the shielding
    G4bool _visibility;

    // Vertex generators
    BoxPointSampler* _cu_box_gen;
    BoxPointSampler*  _cu_external_surf_gen;   
    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} // end namespace nexus

#endif
