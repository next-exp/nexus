// ----------------------------------------------------------------------------
///  \file   
///  \brief    Copper castle inside the shielding lead 
///  \author   <miquel.nebot@ific.uv.es>,
///  \date     3 Feb 2014
///  \version  $Id$
///
///  Copyright (c) 2014 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_MINI_CASTLE__
#define __NEXTNEW_MINI_CASTLE__

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

class G4GenericMessenger;

namespace nexus {

  class NextNewMiniCastle: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewMiniCastle();

    /// Destructor
    ~NextNewMiniCastle();

    /// Sets the Logical Volume where Radon tube will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();

    /// The position of the surface of the pedestal is needed
    /// for the castle to lay on it.
    void SetPedestalSurfacePosition(G4double ped_surf_pos);


  private:
    // Mother Logical Volume of the ICS
    G4LogicalVolume* _mother_logic;
    // Dimensions
    G4double  _x, _y, _z, _thickness;
   
    // Visibility of the shielding
    G4bool _visibility;

    // Vertex generators
    BoxPointSampler* _mini_castle_box_gen;
    BoxPointSampler* _mini_castle_external_surf_gen;
    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg;

    G4double _pedestal_surf_y;

  };

} // end namespace nexus

#endif
