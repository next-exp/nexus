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

#ifndef __NEXT100_VESSEL__
#define __NEXT100_VESSEL__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"
#include "SpherePointSampler.h"

#include <G4Navigator.hh>


class G4GenericMessenger;

namespace nexus {

  class Next100Vessel: public BaseGeometry
  {
  public:
    /// Constructor
    Next100Vessel(const G4double nozzle_ext_diam,
		  const G4double up_nozzle_ypos,
		  const G4double central_nozzle_ypos,
		  const G4double down_nozzle_ypos,
		  const G4double bottom_nozzle_ypos);

    /// Destructor
    ~Next100Vessel();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Returns the logical volume of the inner object
    G4LogicalVolume* GetInternalLogicalVolume();

    /// Builder
    void Construct();



  private:
    // Dimensions
    G4double _vessel_in_rad, _vessel_body_length, _vessel_length, _vessel_thickness;
    G4double _endcap_in_rad, _endcap_theta, _endcap_thickness, _endcap_in_z_width;
    G4double _flange_out_rad, _flange_length, _flange_z_pos;
    G4double _large_nozzle_length, _small_nozzle_length;
    G4double _sc_yield;
    G4double _pressure, _temperature;
   
    // Visibility of the shielding
    G4bool _visibility;

    // Dimensions coming from outside  
    G4double _nozzle_ext_diam, _up_nozzle_ypos, _central_nozzle_ypos;
    G4double _down_nozzle_ypos, _bottom_nozzle_ypos;


    // Internal Logical Volume
    G4LogicalVolume* _internal_logic_vol;

    // Vertex generators
    CylinderPointSampler* _body_gen;
    SpherePointSampler*   _tracking_endcap_gen;
    SpherePointSampler*   _energy_endcap_gen;
    CylinderPointSampler* _tracking_flange_gen;
    CylinderPointSampler* _energy_flange_gen;

    G4double _perc_endcap_vol;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} // end namespace nexus

#endif
