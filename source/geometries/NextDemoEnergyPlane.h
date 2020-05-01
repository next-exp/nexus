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

    /*/// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;
    */

    // Builder
    void Construct();


  private:
    void GeneratePositions();

  private:

    // Mother Logical Volume of the whole Energy PLane
    G4LogicalVolume* _mother_logic;
    G4Material* _gas;
    G4double _pressure, _temperature;

    // Dimensions
    const G4int _num_PMTs;
    const G4double _energy_plane_posz;
    const G4double _carrier_plate_thickness, _carrier_plate_diam, _carrier_plate_central_hole_diam;
    const G4double _enclosure_length, _enclosure_diam;
    const G4double _enclosure_flange_length;
    const G4double _enclosure_window_thickness,_enclosure_window_diam;
    const G4double _enclosure_pad_thickness;
    const G4double _pmt_base_diam, _pmt_base_thickness;
    const G4double _tpb_thickness;


    // Visibility
    G4bool _visibility, _verbosity;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    // PMT
    PmtR11410*  _pmt;
    std::vector<G4ThreeVector> _pmt_positions;
    G4double _pmt_zpos;


    


  };

} //end namespace nexus
#endif
