// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     18 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_INNER_ELEMENTS__
#define __NEXTNEW_INNER_ELEMENTS__

#include "NextNewFieldCage.h"
#include "NextNewEnergyPlane.h"
#include "NextNewTrackingPlane.h"
#include "CylinderPointSampler.h"

#include <G4ThreeVector.hh>

class G4LogicalVolume;
class G4Material;
class G4GenericMessenger;

namespace nexus {

  /// This is a geometry placer that encloses from the FIELD CAGE to inside
  /// covering the ACTIVE region, EL gap and its grids, the cathode grid ...

  class NextNewInnerElements{

  public:
    // Constructor
    NextNewInnerElements();
    // Destructor
    ~NextNewInnerElements();
    
    // Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    // It Returns the relative position respect to the rest of NEXTNEW geometry
    G4ThreeVector GetPosition() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();

  private:
    
    G4LogicalVolume* _mother_logic;
    G4Material* _gas;
    G4double _pressure;
    G4double _temperature;

    void BuildELRegion();
    void BuildCathodeGrid();
    void BuildActive();    

    G4double _max_step_size;
    
    G4double _active_diam;
    G4double _active_length;
    G4double _tracking_displ, _energy_displ; // Displacement of tracking / energy zones
    G4double _el_gap;
    G4double _grid_thickness;
    G4double _el_grid_transparency, _cathode_grid_transparency;

    G4double _el_grid_ref_z;//?????????????
    
    // Detector parts
    NextNewFieldCage*     _field_cage;
    NextNewEnergyPlane*   _energy_plane;
    NextNewTrackingPlane* _tracking_plane;

    // Visibilities
    G4bool _grids_visibility;

    // Vertex Generators
    CylinderPointSampler* _active_gen;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} // end namespace nexus

#endif
    
