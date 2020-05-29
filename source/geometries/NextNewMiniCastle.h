// ----------------------------------------------------------------------------
///  \file   
///  \brief    Copper castle inside the shielding lead 
///  \author   <miquel.nebot@ific.uv.es>, <paola.ferrario@dipc.org>
///  \date     3 Feb 2014
///  \version  $Id$
///
///  Copyright (c) 2014 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef _NEXTNEW_MINI_CASTLE___
#define _NEXTNEW_MINI_CASTLE___

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
    // Mother logical volume
    G4LogicalVolume* mother_logic_;

    // Dimensions
    G4double  x_, y_, z_, thickness_, _open_spacez_;
    G4double steel_thickn_;

    // Vertex generators
    BoxPointSampler* mini_castle_box_gen_;
    BoxPointSampler* mini_castle_external_surf_gen_;
    BoxPointSampler* steel_box_gen_;
    
    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Position of the pedestal surface in y
    G4double pedestal_surf_y_;

  };

} // end namespace nexus

#endif
