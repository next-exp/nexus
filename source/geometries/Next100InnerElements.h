// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <jmunoz@ific.uv.es>
///  \date     2 Mar 2012
///  \version  $Id$
///
///  Copyright (c) 2012 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100_INNER_ELEMENTS__
#define __NEXT100_INNER_ELEMENTS__

#include "Next100FieldCage.h"
#include "Next100EnergyPlane.h"
#include "Next100TrackingPlane.h"
#include "CylinderPointSampler.h"

#include <G4ThreeVector.hh>


class G4LogicalVolume;
class G4Material;
class G4GenericMessenger;

namespace nexus {

  /// This is a geometry placer that encloses from the FIELD CAGE to inside
  /// covering the ACTIVE region, EL gap and its grids, the cathode grid ...

  class Next100InnerElements{

  public:
    // Constructor
    Next100InnerElements();

    // Destructor
    ~Next100InnerElements();

    // Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    // It Returns the relative position respect to the rest of NEXT100 geometry
    G4ThreeVector GetPosition() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:

    void CalculateELTableVertices(G4double radius, G4double binning,  G4double z);	
    
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
    G4double _trk_displ, _ener_displ; // Displacement of tracking / energy zones
    G4double _el_gap_length;
    G4double _grid_thickn;
    G4double _el_grid_transparency, _cath_grid_transparency;
    // True if EL field is on
    G4bool _elfield;
    // Pitch of the EL grid generation points
    G4double  _el_pitch;
    G4double _el_gap_posz;

    G4double _el_grid_ref_z;

    // Detector parts
    Next100FieldCage*     _field_cage;
    Next100EnergyPlane*   _energy_plane;
    Next100TrackingPlane* _tracking_plane;


    // Visibilities
    G4bool _grids_visibility;


    // Vertex Generators
    CylinderPointSampler* _active_gen;


    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    // Variables for the EL table generation
    mutable G4int _idx_table;
    mutable std::vector<G4ThreeVector> _table_vertices;

  };

} // end namespace nexus

#endif
