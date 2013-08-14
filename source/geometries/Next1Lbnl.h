// ----------------------------------------------------------------------------
///  \file   Next1Lbnl.h
///  \brief  Geometry of the LBNL detector prototype.
///
///  Author:  F. Monrabal <francesc.monrabal@ific.uv.es>
///  Created: 1 December 2010
/// 
///  Copyright (c) 2010, 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT1_LBNL__
#define __NEXT1_LBNL__

#include "BaseGeometry.h"

#include <iostream>
#include <fstream>
#include <vector>

namespace nexus {

  //class HexagonNavigator;
  

    
  class Next1Lbnl: public BaseGeometry
  {
  public:
    /// Constructor
    Next1Lbnl();
    /// Destructor
    ~Next1Lbnl();

    /// Returns a vertex in a region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;
    G4double returndriftv(G4double Efield, G4double density);
    void CalculateELTableVertices(G4double xlen, G4double ylen, G4double binning, G4double z);

  private:

    void BuildGeometry();
    void DefineDaughterBoards();
    void ReadParameters();

  private:

    G4LogicalVolume* _dbo44_logic; ///< 4x4 daughter-board (DBO) logical volume
    G4LogicalVolume* _dboCU_logic; ///< Corner (up) DBO logical volume
    G4LogicalVolume* _dboCD_logic; ///< Corner (down) DBO logical volume

    G4int _tracking_plane;

    // From the Next1EL geometry
    G4int _PMTID;
    G4int _numb_of_events;
    mutable G4int _idx_table;
    mutable std::vector<G4ThreeVector> _table_vertices;   

    G4double _pressure;
    G4double _max_step_size;
    G4double _binning;              ///< ELtable binning

    G4int _el_on;                   ///< turn on/off EL field
    
  };


} // end namespace nexus

#endif
