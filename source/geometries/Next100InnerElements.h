// ----------------------------------------------------------------------------
///  \file   Next100InnerElements.h
///  \brief  Geometry of the field cage and sensor planes of NEXT-100.
///
///  \author   <jmunoz@ific.uv.es>
///  \date     2 Mar 2012
///  \version  $Id$
///
///  Copyright (c) 2012-2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_INNER_ELEMENTS_H
#define NEXT100_INNER_ELEMENTS_H

#include <G4ThreeVector.hh>
#include <vector>

class G4LogicalVolume;
class G4Material;
class G4GenericMessenger;


namespace nexus {

  class Next100FieldCage;
  class Next100EnergyPlane;
  class Next100TrackingPlane;
  class CylinderPointSampler;


  /// This is a geometry placer that encloses from the FIELD CAGE to inside
  /// covering the ACTIVE region, EL gap and its grids, the cathode grid ...

  class Next100InnerElements{

  public:
    ///Constructor
    Next100InnerElements();

    /// Destructor
    ~Next100InnerElements();

    /// Set the logical volume that encloses the entire geometry
    void SetLogicalVolume(G4LogicalVolume*);

    /// Return the relative position respect to the rest of NEXT100 geometry
    G4ThreeVector GetPosition() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:

    void BuildELRegion();
    void BuildAnodePlate();
    void BuildCathodeGrid();
    void BuildActive();
    void BuildBuffer();

    void CalculateELTableVertices(G4double, G4double, G4double);	
    
    G4LogicalVolume* _mother_logic;
    G4Material* _gas;

    G4double _pressure;
    G4double _temperature;

    G4double _max_step_size;
    
    G4double _active_diam;
    G4double _active_length;
    G4double _windows_end_z;
    G4double _trk_displ, _ener_displ; // Displacement of tracking / energy zones
    G4double _el_gap_length;
    G4double _grid_thickn;
    G4double _el_grid_transparency, _cath_grid_transparency;
    // True if EL field is on
    G4bool _elfield;

    G4double _anode_quartz_thickness, _anode_quartz_diam; 
    G4double _tpb_thickness;
    G4double _ito_transparency, _ito_thickness;

    G4double _el_table_binning; ///< Binning of EL lookup table
    G4int _el_table_point_id; ///< Id of the EL point to be simulated
    mutable G4int _el_table_index; ///< Index for EL lookup table generation

    G4double _el_gap_posz;

    G4double _el_grid_ref_z;

    // Detector parts
    Next100FieldCage*     _field_cage;
    Next100EnergyPlane*   _energy_plane;
    Next100TrackingPlane* _tracking_plane;

    // Visibilities
    G4bool _grids_visibility;

    G4double _cathode_pos_z, _buffer_length;

    // Vertex Generators
    CylinderPointSampler* _active_gen;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    // Variables for the EL table generation
    mutable std::vector<G4ThreeVector> _table_vertices;
  };

} // end namespace nexus

#endif
