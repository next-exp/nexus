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

#ifndef __NEXT100_ICS__
#define __NEXT100_ICS__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"
#include "SpherePointSampler.h"

#include <G4Navigator.hh>

class G4GenericMessenger;


namespace nexus {


  class Next100Ics: public BaseGeometry
  {
  public:
    /// Constructor
    Next100Ics(const G4double nozzle_ext_diam,
	       const G4double up_nozzle_ypos,
	       const G4double central_nozzle_ypos,
	       const G4double down_nozzle_ypos,
	       const G4double bottom_nozzle_ypos);

    /// Destructor
    ~Next100Ics();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:
    // Dimensions
    G4double _body_in_rad, _body_length, _body_thickness;
    G4double _tracking_orad, _tracking_length, _tracking_irad, _tracking_cone_height, _tracking_hole_rad;
    G4double _energy_theta, _energy_orad, _energy_thickness, _energy_sph_zpos, _energy_cyl_length;

    // Dimensions coming from outside  
    G4double _nozzle_ext_diam, _up_nozzle_ypos, _central_nozzle_ypos;
    G4double _down_nozzle_ypos, _bottom_nozzle_ypos;

    // Visibility of the shielding
    G4bool _visibility;

    // Vertex generators
    CylinderPointSampler* _body_gen;
    CylinderPointSampler* _tracking_gen;
    CylinderPointSampler* _energy_cyl_gen;
    SpherePointSampler*   _energy_sph_gen;

    G4double _perc_body_vol, _perc_tracking_vol, _perc_energy_cyl_vol;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} // end namespace nexus

#endif
