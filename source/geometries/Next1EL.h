// ----------------------------------------------------------------------------
///  \file   Next1EL.h
///  \brief  Geometry of the NEXT-1 EL detector.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     16 Feb 2010
///  \version  $Id$
///
///  Copyright (c) 2010, 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT1_EL__
#define __NEXT1_EL__

#include "BaseGeometry.h"
#include <G4RotationMatrix.hh>
#include <vector>

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;
class G4UIcmdWith3VectorAndUnit;


namespace nexus {
  
  class HexagonPointSampler;
  class CylinderPointSampler;

  
  /// Geometry of the NEXT1-EL detector
    
  class Next1EL: public BaseGeometry
  {
  public:
    /// Constructor
    Next1EL();
    /// Destructor
    ~Next1EL();

    /// Returns a vertex in a region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;
    void CalculateELTableVertices(G4double radius, G4double binning, G4double z);
    
  private:
    void Construct();
    void DefineMaterials();
    void BuildLab();
    void BuildExtScintillator();
    void BuildVessel();
    void BuildFieldCage();
    void BuildEnergyPlane();
    void BuildSiPMTrackingPlane();
    void BuildPMTTrackingPlane();
    void PrintAbsoluteSiPMPos();
    
  private:

    HexagonPointSampler* _hexrnd;

    // Pointers to materials definition
    G4Material* _air;       ///< Air
    G4Material* _gxe;       ///< Gaseous xenon
    G4Material* _fgrid;     ///< For grid simulation
    G4Material* _teflon;    ///< PTFE (Teflon)
    G4Material* _steel;     ///< Stainless Steel
    G4Material* _aluminum;  ///< Aluminum
    G4Material* _lead;  ///< Lead
    G4Material* _plastic;  ///< plastic
    G4Material* _tpb; /// tpb


    // Pointers to logical volumes
    G4LogicalVolume* _lab_logic;
    G4LogicalVolume* _gas_logic;
    G4LogicalVolume* _pmt_plane_logic;
    G4LogicalVolume* _pmt_logic;
    
    
    // Detector dimensions
    
    const G4double _lab_size; ///< Size of the air box containing the detector 
    
    const G4double _vessel_diam;   ///< Can internal diameter
    const G4double _vessel_length; ///< Can internal length
    const G4double _vessel_thickn; ///< Can thickness

    const G4double _endcap_diam;   ///< Vessel endcap diameter
    const G4double _endcap_thickn; ///< Vessel endcap thickness

    const G4double _sideport_diam;   ///< Side source-port internal diameter
    const G4double _sideport_length; ///< Side source-port internal length
    const G4double _sideport_thickn; ///< Side source-port thickness

    const G4double _sideport_flange_diam;   ///< Side-port flange diameter
    const G4double _sideport_flange_thickn; ///< Side-port flange thickness

    const G4double _sideport_tube_diam;   ///< Collimation tube internal diameter
    const G4double _sideport_tube_length; ///< Collimation tube internal length
    const G4double _sideport_tube_thickn; ///< Collimation tube thickness
    const G4double _sideport_tube_window_thickn; ///< Tube window thickness
    
    const G4double _axialport_diam;   ///< Axial source-port internal diameter
    const G4double _axialport_length; ///< Axial source-port internal length
    const G4double _axialport_thickn; ///< Axial source-port internal thickness

    const G4double _axialport_flange_diam;   ///< Axial port flange diameter
    const G4double _axialport_flange_thickn; ///< Axial port flange thickness
    
    const G4double _axialport_tube_diam;   ///< Collimation tube int. diameter
    const G4double _axialport_tube_length; ///< Collimation tube int. length
    const G4double _axialport_tube_thickn; ///< Collimation tube thickness
    const G4double _axialport_tube_window_thickn; ///< Tube window thickness

    const G4double _elgap_length; ///< Length of the EL gap
    
    const G4double _elgap_ring_diam;
    const G4double _elgap_ring_thickn;
    const G4double _elgap_ring_height;
    const G4double _wire_diam; ///< diameter of the EL meshes wires

    const G4double _tpb_thickn;

    const G4double _ltube_diam;
    const G4double _ltube_thickn;
    const G4double _ltube_up_length;
    const G4double _ltube_bt_length;
    const G4double _ltube_gap;

    const G4double _active_diam;
    const G4double _active_length;
    
    const G4double _fieldcage_diam;   ///< Fieldcage diameter
    const G4double _fieldcage_length; ///< Fieldcage length
    const G4double _fieldcage_displ;  ///< Distance btw fieldcage & anode endcap

    const G4double _ring_diam;
    const G4double _ring_height;
    const G4double _ring_thickn;

    const G4double _bar_width;
    const G4double _bar_thickn;
    const G4double _bar_addon_length;

    const G4double _pmtholder_cath_diam;
    const G4double _pmtholder_cath_height;
    const G4double _pmtholder_cath_cutout_depth;
    const G4double _pmtholder_cath_displ;
    const G4double _pmt_pitch;
    
    const G4double _pmtholder_anode_diam;
    const G4double _pmtholder_anode_thickn;

   G4double _max_step_size;
   G4double _sc_yield;
    
    
    // Dimensions read through input parameters file
    //

    G4double _sideport_angle;
    G4double _sideport_posz;
    G4bool _tpb_coating;
    G4bool _elfield;
    G4bool _external_scintillator;
    G4double _elgrid_transparency;
    G4double _gate_transparency;
    G4ThreeVector _specific_vertex;
    G4double _specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z;

    
    // Positions in the detector geometry relevant for event vertex generation
    //
    
    /// Position of the side source-port in the world reference system
    G4ThreeVector _sideport_position;
    G4ThreeVector _sideport_ext_position;
    G4ThreeVector _sideNa_pos;
    /// Position of the axial source-port in the world reference system
    G4ThreeVector _axialport_position;
    /// Position of the fieldcage in the world system of reference
    G4ThreeVector _fieldcage_position;
    /// 
    G4ThreeVector _active_position;
    ///
    G4ThreeVector _elgap_position;

    G4String _tracking_plane; ///< Tracking plane type: SIPM or PMT
        

    // Positions (Z axis) of the TPC electrodes with respect to the
    // FIELDCAGE system of reference
    G4double _gate_posz;    ///< Z position of the gate wrt FIELDCAGE
    G4double _anode_posz;   ///< Z position of the anode wrt FIELDCAGE
    G4double _cathode_posz; ///< Z position of the cathode wrt FIELDCAGE

    G4double _pressure;
    

    G4double _endend;
    
    G4int _PMTID;
    G4int _numb_of_events;
    mutable G4int _idx_table;
    mutable std::vector<G4ThreeVector> _table_vertices;   

    std::vector<G4ThreeVector> _pmt_positions;

    //Vector to store the ID number of each SiPM together with its absolute position in gas
    std::vector<std::pair<int, G4ThreeVector> > _absSiPMpos;

    /// Messenger for the definition of control commands
    G4GenericMessenger* _msg; 
    G4UIcmdWith3VectorAndUnit* _specific_v_cmd;

    /// To generate vertices in the Na22 source
    CylinderPointSampler* _cps;
  };


} // end namespace nexus

#endif
