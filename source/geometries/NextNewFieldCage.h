// ----------------------------------------------------------------------------
///  \file     NextNewFieldCage.h  
///  \brief    This is a geometry formed by the reflector tube and  TPB layer if needed.
///            Also build the EL, Cathode and Electric Drift Field. 
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     12 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_FIELDCAGE__
#define __NEXTNEW_FIELDCAGE__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"
#include <vector>

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;

namespace nexus {

  class NextNewFieldCage: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewFieldCage();
    /// Destructor
    ~NextNewFieldCage();
    
    /// Sets the Logical Volume where Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// To calculate the vertices for the EL table generation
    void CalculateELTableVertices(G4double radius, 
				  G4double binning, G4double z);
    ///
    G4ThreeVector GetPosition() const;
    /// Builder
    void Construct();

  private:
    G4bool _elfield;
    G4double _max_step_size;
    // Mother Logical Volume 
    G4LogicalVolume* _mother_logic;
    G4Material* _gas;
    G4double _pressure;
    G4double _temperature;

    // Pointers to materials definition
    G4Material* _hdpe;//High density polyethylene
    
    void BuildELRegion();
    void BuildCathodeGrid();
    void BuildActive();    

    // Dimensions
    G4double _cage_tot_length, _tube_in_diam, _tube_length; 
    G4double _tube_thickness, _tube_z_pos;
    G4double _reflector_thickness;
    G4double _el_gap_z_pos, _el_tot_zone, _el_gap_length, _grid_thickness;
    G4double _el_grid_transparency, _gate_transparency; 
    G4double _cathode_grid_transparency;
    G4double _drift_length;

    // Visibility 
    G4bool _visibility;

    // Vertex generators
    CylinderPointSampler* _field_cage_gen;
    CylinderPointSampler* _reflector_gen;
    CylinderPointSampler* _active_gen;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    mutable G4int _idx_table;
    mutable std::vector<G4ThreeVector> _table_vertices;
  };

} //end namespace nexus
#endif
