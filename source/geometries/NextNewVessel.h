// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <miquel.nebot@ific.uv.es>, <jmunoz@ific.uv.es>, <justo.martin-albo@ific.uv.es>
///  \date     9 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_VESSEL__
#define __NEXTNEW_VESSEL__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"
#include "SpherePointSampler.h"
//#include "BoxPointSampler.h"

#include <G4Navigator.hh>


class G4GenericMessenger;

namespace nexus {

  class NextNewVessel: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewVessel();
    ///Destructor
    ~NextNewVessel();


    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Returns the logical volume of the inner object
    G4LogicalVolume* GetInternalLogicalVolume();

    /// Builder
    void Construct();


  private:
    //Dimensions
    G4double  _vessel_in_rad, _vessel_body_length, _vessel_length, _vessel_thickness;
    G4double _endcap_in_rad, _endcap_theta, _endcap_thickness, _endcap_in_z_width;
    G4double _flange_out_rad, _flange_length, _flange_z_pos;

    G4double _pressure, _temperature;

    // Visibility of the shielding
    G4bool _visibility;

    // Internal Logical Volume
    G4LogicalVolume* _internal_logic_vol;

    // Vertex generators
    //BoxPointSampler* _lab_gen;????????
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
 
