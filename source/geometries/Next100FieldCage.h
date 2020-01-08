// ----------------------------------------------------------------------------
///  \file
///  \brief
///
///  \author   <paola.ferrario@dipc.org>
///  \date     1 Mar 2012
///  \version  $Id$
///
///  Copyright (c) 2012-2020 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100_FIELDCAGE__
#define __NEXT100_FIELDCAGE__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"


class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

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

  private:

    void DefineMaterials();
    // void BuildCathodeGrid();
    void BuildActive();
    void BuildBuffer();
    // void BuildELRegion();
    // void BuildAnodeGrid();
    void BuildFieldCage();

    void CalculateELTableVertices(G4double, G4double, G4double);

    // Dimensions
    const G4double _active_diam, _active_length, _buffer_length;
    const G4double _grid_thickn;
    const G4double _teflon_drift_length, _teflon_buffer_length, _teflon_thickn;
    const G4int _npanels;
    const G4double _tpb_thickn;
    G4double _drift_transv_diff, _drift_long_diff;
    G4double _active_zpos;


    // Visibility of the geometry
    G4bool _visibility;
    // Verbosity of the geometry
    G4bool _verbosity;


    // Vertex generators
    //CylinderPointSampler* _active_gen;


    // Messenger for the definition of control commands
    G4GenericMessenger* _msg;

    // Logical volume where the class is placed
    G4LogicalVolume* _mother_logic;
    G4Material* _gas;
    G4double _pressure;
    G4double _temperature;

    // Pointers to materials definition
    G4Material* _hdpe;
    G4Material* _tpb;
    G4Material* _ito;
    G4Material* _teflon;
    G4Material* _copper;

    G4double _max_step_size;

  };

} //end namespace nexus
#endif
