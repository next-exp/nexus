// ----------------------------------------------------------------------------
///  \file     NextNewFieldCage.h  
///  \brief    Reflector tube coated with TPB layer. 
///            Also builds the EL, Cathode and Electric Drift Field. 
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     12 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013-2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXTNEW_FIELDCAGE_H
#define NEXTNEW_FIELDCAGE_H

#include "BaseGeometry.h"
#include <vector>

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;
class G4VisAttributes;


namespace nexus {

    class CylinderPointSampler;


  class NextNewFieldCage: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewFieldCage();
    /// Destructor
    ~NextNewFieldCage();
    
    /// Sets the Logical Volume where Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generates a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Gives the absolute position of the field cage ensemble
    G4ThreeVector GetPosition() const;

    /// Builder
    void Construct();

  private:
    void DefineMaterials();
    void BuildELRegion();
    void BuildCathodeGrid();
    void BuildActive(); 
    void BuildFieldCage(); 
    void BuildAnodeGrid();

    /// Calculates the vertices for the EL table generation
    void CalculateELTableVertices(G4double radius, 
				  G4double binning, G4double z);

  private:
   
    // Mother Logical Volume 
    G4LogicalVolume* _mother_logic;
    G4Material* _gas;
    G4double _pressure;
    G4double _temperature;

    // Pointers to materials definition
    G4Material* _hdpe; 
    G4Material* _tpb;
    G4Material* _ito;
    G4Material* _teflon; 
    G4Material* _quartz;

    // Dimensions
    G4double _dist_feedthroughs, _cathode_thickness, _cathode_gap;
    G4double _buffer_length;
    G4double _tube_in_diam, _tube_length_drift, _dist_tube_el; //_tube_length_buff 
    G4double _tube_thickness, _tube_z_pos;
    G4double _reflector_thickness, _tpb_thickness;
    G4double _el_gap_z_pos, _el_gap_length, _grid_thickness;
    G4double _el_grid_transparency, _gate_transparency; 
    G4double _anode_quartz_thickness, _anode_quartz_diam; //_anode_quartz_transparency
    G4double _pos_z_anode;
    G4double _cathode_grid_transparency;
    G4double  _ito_transparency, _ito_thickness;

    // Visibility 
    G4bool _visibility;
    G4VisAttributes* _grey_color;
    G4VisAttributes* _red_color;
    G4VisAttributes* _light_blue_color;
    G4VisAttributes* _blue_color;
    G4VisAttributes* _green_color;

    G4double _max_step_size;
    G4bool _elfield;
    
    // Vertex generators
    CylinderPointSampler* _drift_tube_gen;
    CylinderPointSampler* _reflector_drift_gen;
    CylinderPointSampler* _buffer_tube_gen;
    CylinderPointSampler* _reflector_buffer_gen;
    CylinderPointSampler* _active_gen;
    CylinderPointSampler* _anode_quartz_gen;

    G4double _specific_vertex_X;
    G4double _specific_vertex_Y;
    G4double _specific_vertex_Z;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    G4int _el_table_point_id;
    mutable G4int _el_table_index;
    mutable std::vector<G4ThreeVector> _el_table_vertices;
    G4double _el_table_binning;
  };

} //end namespace nexus
#endif
