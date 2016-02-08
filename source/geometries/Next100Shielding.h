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

class G4GenericMessenger;



namespace nexus {

  class Next100Shielding: public BaseGeometry
  {
  public:
    /// Constructor
    Next100Shielding();

    /// Destructor
    ~Next100Shielding();

    // Returns the inner air logical volume to place the vessel into it
    G4LogicalVolume* GetAirLogicalVolume() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


    /// Builder
    void Construct();

    
  private:

    // Dimensions
    G4double _shield_x, _shield_y, _shield_z;
    G4double  _beam_base_thickness, _lateral_z_separation, _roof_z_separation , _front_x_separation;
    G4double _lead_thickness, _steel_thickness;

    // Visibility of the shielding
    G4bool _visibility;

    // Vertex generators
    BoxPointSampler* _lead_gen;
    BoxPointSampler* _steel_gen;
    BoxPointSampler* _external_gen;
    BoxPointSampler* _lat_roof_gen;
    BoxPointSampler* _front_roof_gen;
    BoxPointSampler* _struct_x_gen;
    BoxPointSampler* _struct_z_gen; 
    BoxPointSampler* _lat_beam_gen;
    BoxPointSampler* _front_beam_gen;

    G4double _perc_roof_vol; 
    G4double _perc_front_roof_vol;
    G4double _perc_top_struct_vol;
    G4double _perc_struc_x_vol;
 

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    // Air logical volume
    G4LogicalVolume* _air_box_logic;

  };

} // end namespace nexus

#endif
