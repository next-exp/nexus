// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <jmunoz@ific.uv.es>
///  \date     25 Apr 2012
///  \version  $Id$
///
///  Copyright (c) 2012 NEXT Collaboration
//
//  Updated to NextDemo++  by  Ruth Weiss Babai <ruty.wb@gmail.com>
//  From:   "Next100EnergyPlane.h"
//  Date:       June 2019
// ----------------------------------------------------------------------------

#ifndef __NEXT_DEMO_ENERGY_PLANE__
#define __NEXT_DEMO_ENERGY_PLANE__

#include <vector>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

#include "CylinderPointSampler.h"
#include "PmtR11410.h"


class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  /// This is a class to place all the components of the energy plane

  class NextDemoEnergyPlane: public BaseGeometry
  {

  public:
    /// Constructor
    NextDemoEnergyPlane();

    /// Destructor
    ~NextDemoEnergyPlane();

    /// Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Sets the z position of the surface of the sapphire windows
    void SetSapphireSurfaceZPos(G4double z);

    /*/// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;
    */

    // Builder
    void Construct();


  private:
    void GeneratePositions();

  private:

    // Mother Logical Volume of the whole Energy PLane
    G4LogicalVolume* mother_logic_;
    G4Material* gas_;
    G4double pressure_, temperature_;
    G4double end_of_sapphire_posz_;

    // Dimensions
    const G4double dist_gate_support_surface_;
    const G4int num_PMTs_;
    //const G4double energy_plane_posz_;
    const G4double carrier_plate_thickness_, carrier_plate_diam_;
    const G4double carrier_plate_central_hole_diam_;
    const G4double pmt_hole_length_, pmt_hole_diam_;
    const G4double pmt_flange_length_;
    const G4double sapphire_window_thickness_, sapphire_window_diam_;
    const G4double pedot_coating_thickness_;
    const G4double optical_pad_thickness_;
    const G4double pmt_base_diam_, pmt_base_thickness_;
    const G4double tpb_thickness_;


    // Visibility
    G4bool visibility_, verbosity_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

    // PMT
    G4RotationMatrix* pmt_rot_;
    PmtR11410*  pmt_;
    std::vector<G4ThreeVector> pmt_positions_;
    G4double pmt_zpos_;


    


  };

} //end namespace nexus
#endif
