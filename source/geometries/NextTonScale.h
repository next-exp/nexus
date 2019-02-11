// -----------------------------------------------------------------------------
// File   : NextTonScale.h
// Info   : Detector geometry for ton-scale sensitivity studies.
// Author : Justo Martin-Albo
// Date   : January 2019
// -----------------------------------------------------------------------------

#ifndef NEXT_TON_SCALE_H
#define NEXT_TON_SCALE_H

#include "BaseGeometry.h"

class G4GenericMessenger;
class G4Material;

namespace nexus {

  class CylinderPointSampler;

  class NextTonScale: public BaseGeometry
  {
  public:
    // Constructor
    NextTonScale();

    // Destructor
    virtual ~NextTonScale();

    virtual void Construct();
    virtual G4ThreeVector GenerateVertex(const G4String&) const;

  private:
    G4LogicalVolume* ConstructWaterTank(G4LogicalVolume*);
    G4LogicalVolume* ConstructVessel(G4LogicalVolume*);
    G4LogicalVolume* ConstructInnerShield(G4LogicalVolume*);
    G4LogicalVolume* ConstructFieldCage(G4LogicalVolume*);

  private:
    G4GenericMessenger* msg_;

    G4Material* xenon_gas_;

    // Geometry Parameters
    G4double gas_density_;
    G4double active_diam_, active_length_;
    G4double fcage_thickn_, ics_thickn_, vessel_thickn_;
    G4double endcap_hollow_;
    G4double water_thickn_;

    // Vertex Generators
    G4double specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_;

    CylinderPointSampler* active_gen_;
    CylinderPointSampler* field_cage_gen_;
    CylinderPointSampler* ics_gen_;
    CylinderPointSampler* vessel_gen_;
    CylinderPointSampler* readout_plane_gen_;
    CylinderPointSampler* outer_plane_gen_;

    // Visibilities
    G4bool tank_visibility_;
    G4bool vessel_visibility_;
    G4bool ics_visibility_;
    G4bool fcage_visibility_;
  };

} // namespace nexus

#endif
