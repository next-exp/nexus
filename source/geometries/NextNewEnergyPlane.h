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
#include "Enclosure.h"


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
    void GeneratePMTsPositions();
    void GenerateGasHolePositions();

    // Mother Logical Volume of the whole Energy PLane
    G4LogicalVolume* _mother_logic;
    // Dimensions
    const G4double _energy_plane_z_pos;
    const G4double _carrier_plate_front_buffer_thickness,_carrier_plate_front_buffer_diam ;
    const G4double _carrier_plate_thickness, _carrier_plate_diam,_enclosure_hole_diam,_gas_hole_diam,_gas_hole_pos ;
    
    const G4int _num_PMTs;
    const G4int _num_gas_holes;
   
    Enclosure*  _enclosure;
    G4double _enclosure_z_pos;
    std::vector<G4ThreeVector> _pmt_positions;
    std::vector<G4ThreeVector> _gas_hole_positions;
     
    // Visibility of the energy  plane
    G4bool _visibility;

    // Vertex generators
    CylinderPointSampler* _carrier_gen;
    
    // Geometry Navigator
    G4Navigator* _geom_navigator;
    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} //end namespace nexus
#endif
