// ----------------------------------------------------------------------------
///  \file
///  \brief
///
///  \author   <jmunoz@ific.uv.es>, <paola.ferrario@dipc.org>
///  \date     1 Mar 2012
///  \version  $Id$
///
///  Copyright (c) 2012-2020 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100_FIELDCAGE__
#define __NEXT100_FIELDCAGE__

#include "BaseGeometry.h"

#include <G4Navigator.hh>
#include <vector>

class G4Material;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4GenericMessenger;

namespace nexus {

  class CylinderPointSampler2020;

  /// This is a geometry formed by the reflector tube and
  /// TPB layer if needed

  class Next100FieldCage: public BaseGeometry
  {

  public:
    /// Constructor
    Next100FieldCage();
    /// Destructor
    ~Next100FieldCage();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    ///
    G4ThreeVector GetActivePosition() const;
    G4double GetDistanceGateSapphireWindows() const;

    /// Builder
    void Construct();

    /// Sets as mother volume of all the elements the volume where the class is placed
    void SetMotherLogicalVolume(G4LogicalVolume* mother_logic);
    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);

  private:

    void DefineMaterials();
    void BuildCathodeGrid();
    void BuildActive();
    void BuildBuffer();
    void BuildELRegion();
    void BuildFieldCage();

    void CalculateELTableVertices(G4double, G4double, G4double);

    // Dimensions
    const G4double _active_diam, _active_length, _buffer_length;
    const G4double _grid_thickn, _cathode_gap;
    const G4double _teflon_drift_length, _teflon_buffer_length, _teflon_thickn;
    const G4int _n_panels;
    const G4double _tpb_thickn, _el_gap_length;
    // Diffusion constants
    G4double _drift_transv_diff, _drift_long_diff;
    G4double _ELtransv_diff; ///< transversal diffusion in the EL gap
    G4double _ELlong_diff; ///< longitudinal diffusion in the EL gap
    // Electric field
    G4bool _elfield;
    G4double _ELelectric_field; ///< electric field in the EL region
    // Transparencies of grids
    const G4double _cath_grid_transparency, _el_grid_transparency;

    //Step size
    G4double _max_step_size;

    // Variables for the EL table generation
    G4double _el_table_binning; ///< Binning of EL lookup table
    G4int _el_table_point_id; ///< Id of the EL point to be simulated
    mutable G4int _el_table_index; ///< Index for EL lookup table generation
    mutable std::vector<G4ThreeVector> _table_vertices;

    // Visibility of the geometry
    G4bool _visibility;
    // Verbosity of the geometry
    G4bool _verbosity;

    G4double _active_zpos, _el_gap_diam, _el_gap_zpos, _cathode_grid_zpos;
    G4double _active_ext_radius;


    // Vertex generators
    CylinderPointSampler2020* _active_gen;
    CylinderPointSampler2020* _buffer_gen;
    CylinderPointSampler2020* _teflon_gen;
    CylinderPointSampler2020* _xenon_gen;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg;

    // Logical volume where the class is placed
    G4LogicalVolume* _mother_logic;
    G4VPhysicalVolume* _mother_phys;
    G4Material* _gas;
    G4double _pressure;
    G4double _temperature;

    G4double _sc_yield;
    G4double _attachment;

    // Pointers to materials definition
    G4Material* _hdpe;
    G4Material* _tpb;
    G4Material* _teflon;
    G4Material* _copper;

  };

} //end namespace nexus
#endif
