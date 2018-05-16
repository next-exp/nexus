// ----------------------------------------------------------------------------
///  \file   Enclosure.h
///  \brief  Geometry model of the PMTs R11410 enclosure, aka "PMT cans".
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     24 Sept 2013
///  \version  $Id:
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __ENCLOSURE__
#define __ENCLOSURE__

#include <G4ThreeVector.hh>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>
#include "BaseGeometry.h"
#include "CylinderPointSampler.h"
#include "PmtR11410.h"

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;
class G4OpticalSurface;

class G4GenericMessenger;

namespace nexus{
  class Enclosure: public BaseGeometry
  {
  public:
    //Constructor
    Enclosure();
    //Destructor
    ~Enclosure();
 
    G4ThreeVector GetObjectCenter();
    G4double GetWindowDiameter();
   

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    // Builder
    void Construct();

  private:
    //Dimensions
    const G4double _enclosure_in_diam, _enclosure_length, _enclosure_thickness, _enclosure_endcap_diam, _enclosure_endcap_thickness;
    const G4double _enclosure_window_diam, _enclosure_window_thickness, _enclosure_pad_thickness;
    //   const G4double _enclosure_tpb_thickness;
    const G4double _pmt_base_diam, _pmt_base_thickness,_pmt_base_z;

    PmtR11410*  _pmt;
    G4double _pmt_z_pos, _window_z_pos;

    // Visibility of the tracking plane
    G4bool _visibility;

    // Vertex generators
    CylinderPointSampler* _enclosure_body_gen;
    CylinderPointSampler* _enclosure_flange_gen;
    CylinderPointSampler* _enclosure_cap_gen;
    CylinderPointSampler* _enclosure_window_gen;
    CylinderPointSampler* _enclosure_pad_gen;
    CylinderPointSampler* _pmt_base_gen;
    CylinderPointSampler* _enclosure_surf_gen;

    
    G4double _body_perc;
    G4double _flange_perc;
    
    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} //end namespace nexus
#endif
