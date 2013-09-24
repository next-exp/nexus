// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     13 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_ENERGY_PLANE__
#define __NEXTNEW_ENERGY_PLANE__

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
  class NextNewEnergyPlane: public BaseGeometry 
  {

  public:
    /// Constructor
    NextNewEnergyPlane();

    /// Destructor
    ~NextNewEnergyPlane();

    /// Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    // Builder
    void Construct();

  private:
    void GeneratePositions();

    // Mother Logical Volume of the whole Energy PLane
    G4LogicalVolume* _mother_logic;
    // Dimensions
    const G4double _energy_plane_z_pos;
    const G4double _carrier_plate_thickness, _carrier_plate_diam;
    const G4double _enclosure_in_diam, _enclosure_length, _enclosure_thickness, _enclosure_endcap_diam, _enclosure_endcap_thickness; 
    const G4double _enclosure_window_thickness, _enclosure_pad_thickness; 
   
    const G4int _num_PMTs;
    PmtR11410*  _pmt;
    std::vector<G4ThreeVector> _pmt_positions;
    G4double _pmt_z_pos;
     
    // Visibility of the tracking plane
    G4bool _visibility;

    // Vertex generators
    CylinderPointSampler* _carrier_gen;
    CylinderPointSampler* _enclosure_body_gen;
    CylinderPointSampler* _enclosure_cap_gen; 
    CylinderPointSampler* _enclosure_window_gen; 

    G4double _enclosure_body_perc; 
    
    // Geometry Navigator
    G4Navigator* _geom_navigator;
    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} //end namespace nexus
#endif
