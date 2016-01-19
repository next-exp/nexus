// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <jmunoz@ific.uv.es>
///  \date     25 Apr 2012
///  \version  $Id$
///
///  Copyright (c) 2012 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100_ENERGY_PLANE__
#define __NEXT100_ENERGY_PLANE__

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

  class Next100EnergyPlane
  {

  public:
    /// Constructor
    Next100EnergyPlane();

    /// Destructor
    ~Next100EnergyPlane();

    /// Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    // Builder
    void Construct();


  private:
    void GeneratePositions();

  private:

    // Mother Logical Volume of the whole Energy PLane
    G4LogicalVolume* _mother_logic;

    // Dimensions
    const G4int _num_PMTs;
    const G4double _energy_plane_posz;
    const G4double _carrier_plate_thickness, _carrier_plate_diam, _carrier_plate_central_hole_diam;
    const G4double _enclosure_length, _enclosure_diam;
    const G4double _enclosure_flange_length;
    const G4double _enclosure_window_thickness,_enclosure_window_diam;
    const G4double _enclosure_pad_thickness;
    const G4double _pmt_base_diam, _pmt_base_thickness;
    // const G4double _pmts_pitch;

    PmtR11410*  _pmt;
    std::vector<G4ThreeVector> _pmt_positions;
    G4double _pmt_zpos;


    // Visibility of the tracking plane
    G4bool _visibility;


    // Vertex generators
    CylinderPointSampler* _carrier_gen;
    CylinderPointSampler* _enclosure_flange_gen;
    CylinderPointSampler* _enclosure_window_gen;
    CylinderPointSampler* _enclosure_pad_gen;
    CylinderPointSampler* _pmt_base_gen;

    G4double _enclosure_body_perc;
    
    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} //end namespace nexus
#endif
