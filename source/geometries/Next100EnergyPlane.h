// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <jmunoz@ific.uv.es>
///  \date     25 Apr 2012
///  \version  $Id$
///
///  Copyright (c) 2012 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100_ENERGY_PLANE__
#define __NEXT100_ENERGY_PLANE__

#include <vector>
#include <G4Navigator.hh>
#include <G4RotationMatrix.hh>

#include "CylinderPointSampler.h"
#include "PmtR11410.h"

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  /// This is a class to place all the components of the energy plane

  class Next100EnergyPlane: public BaseGeometry
  {

  public:
    /// Constructor
    Next100EnergyPlane();

    /// Destructor
    ~Next100EnergyPlane();

    /// Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    // Builder
    void Construct();


  private:
    void GeneratePositions();

  private:

    // Mother Logical Volume of the whole Energy PLane
    G4LogicalVolume* _mother_logic;

    // Dimensions
    const G4int _num_PMTs;
    const G4double _end_of_sapphire_posz;
    const G4double _copper_plate_thickness, _copper_plate_diam,
      _copper_plate_central_hole_diam;
    const G4double _hut_thickn, _hut_vacuum_length;
    const G4double _hut_length_short, _hut_length_medium, _hut_length_long;
    const G4double _hole_diam;
    
    //const G4double _enclosure_length, _enclosure_diam;
    //const G4double _enclosure_flange_length;
    G4double _vacuum_length;
    const G4double _sapphire_window_thickness,_sapphire_window_diam;
    const G4double _optical_pad_thickness;
    //const G4double _pmt_base_diam, _pmt_base_thickness;
    const G4double _tpb_thickness;
    // const G4double _pmts_pitch;

    PmtR11410*  _pmt;
    std::vector<G4ThreeVector> _pmt_positions;
    std::vector<G4ThreeVector> _short_hut_pos, _medium_hut_pos, _long_hut_pos;
    G4double _pmt_zpos;
    G4RotationMatrix* _pmt_rot;
    G4double _rot_angle;


    // Visibility of the tracking plane
    G4bool _visibility;


    // Vertex generators
    CylinderPointSampler* _copper_gen;
    //CylinderPointSampler* _enclosure_flange_gen;
    CylinderPointSampler* _sapphire_window_gen;
    CylinderPointSampler* _optical_pad_gen;

    //  G4double _enclosure_body_perc;
    
    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} //end namespace nexus
#endif
