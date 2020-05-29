// ----------------------------------------------------------------------------
///  \file   NextNewPmtEnclosure.h
///  \brief  Geometry model of the PMTs R11410 enclosure, aka "PMT cans".
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     24 Sept 2013
///  \version  $Id:
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef _ENCLOSURE___
#define _ENCLOSURE___

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
  class NextNewPmtEnclosure: public BaseGeometry
  {
  public:
    //Constructor
    NextNewPmtEnclosure();
    //Destructor
    ~NextNewPmtEnclosure();
 
    G4ThreeVector GetObjectCenter();
    G4double GetWindowDiameter();
   

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    // Builder
    void Construct();

  private:
    //Dimensions
    const G4double enclosure_in_diam_, enclosure_length_, enclosure_thickness_, enclosure_endcap_diam_, enclosure_endcap_thickness_;
    const G4double enclosure_window_diam_, enclosure_window_thickness_, enclosure_pad_thickness_;
    //   const G4double enclosure_tpb_thickness_;
    const G4double pmt_base_diam_, pmt_base_thickness_,pmt_base_z_;

    PmtR11410*  pmt_;
    G4double pmt_z_pos_, window_z_pos_, gas_pos_;

    // Visibility of the tracking plane
    G4bool visibility_;

    // Vertex generators
    CylinderPointSampler* enclosure_body_gen_;
    CylinderPointSampler* enclosure_flange_gen_;
    CylinderPointSampler* enclosure_cap_gen_;
    CylinderPointSampler* enclosure_window_gen_;
    CylinderPointSampler* enclosure_pad_gen_;
    CylinderPointSampler* pmt_base_gen_;
    CylinderPointSampler* enclosure_surf_gen_;
    CylinderPointSampler* enclosure_cap_surf_gen_;

    
    G4double body_perc_;
    G4double flange_perc_;
    G4double int_surf_perc_, int_cap_surf_perc_;
    
    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

  };

} //end namespace nexus
#endif
