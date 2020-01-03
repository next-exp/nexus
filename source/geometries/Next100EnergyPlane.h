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
    const G4double _copper_plate_thickn, _copper_plate_diam;
    const G4double _copper_plate_central_hole_diam;
    const G4double _hut_int_diam, _hut_thickn, _hut_hole_length;
    const G4double _hut_length_short, _hut_length_medium, _hut_length_long;
    const G4double _hole_diam_front, _hole_diam_rear;
    const G4double _hole_length_front, _hole_length_rear;
    const G4double _sapphire_window_thickn, _optical_pad_thickn, _tpb_thickn;
    const G4double _pmt_stand_out;

    // Visibility of the energy plane
    G4bool _visibility, _verbosity;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg;

    // PMT
    PmtR11410*  _pmt;

    G4double _copper_plate_posz;
    G4double _vacuum_posz;
    //const G4double _pmt_base_diam, _pmt_base_thickn;
    std::vector<G4ThreeVector> _pmt_positions;
    std::vector<G4ThreeVector> _short_hut_pos, _medium_hut_pos, _long_hut_pos;
    G4double _pmt_zpos;
    G4RotationMatrix* _pmt_rot;
    G4double _rot_angle;

    // Vertex generators
    CylinderPointSampler* _copper_gen;
    //CylinderPointSampler* _enclosure_flange_gen;
    CylinderPointSampler* _sapphire_window_gen;
    CylinderPointSampler* _optical_pad_gen;

  };

} //end namespace nexus
#endif
