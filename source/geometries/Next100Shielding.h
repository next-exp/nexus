// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>, <jmunoz@ific.uv.es>
///  \date     21 Nov 2011
///  \version  $Id$
///
///  Copyright (c) 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100_SHIELDING__
#define __NEXT100_SHIELDING__

#include "BaseGeometry.h"
#include "BoxPointSampler.h"
#include <G4Navigator.hh>


namespace nexus {

  class Next100Shielding: public BaseGeometry
  {
  public:
    /// Constructor
    Next100Shielding(const G4double nozzle_ext_diam,
		     const G4double up_nozzle_ypos,
		     const G4double central_nozzle_ypos,
		     const G4double down_nozzle_ypos,
		     const G4double bottom_nozzle_ypos);

    /// Destructor
    ~Next100Shielding();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Returns the logical volume of the inner object
    /// G4LogicalVolume* GetInternalLogicalVolume();

  private:
    void ReadParameters();

    
  private:

    // Dimensions
    G4double _shield_x, _shield_y, _shield_z;
    G4double _lead_thickness, _steel_thickness;

    // Visibility of the shielding
    G4int _visibility;

    // Internal Logical Volume
    G4LogicalVolume* _internal_logic_vol;

    // Vertex generators
    BoxPointSampler* _lead_gen;
    BoxPointSampler* _steel_gen;
    BoxPointSampler* _external_gen;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

  };

} // end namespace nexus

#endif
