// -----------------------------------------------------------------------------
// File   : NextTonScale.h
// Info   : Detector geometry for ton-scale sensitivity studies.
// Author : Justo Martin-Albo
// Date   : July 2019
// -----------------------------------------------------------------------------

#ifndef NEXT_TON_SCALE_H
#define NEXT_TON_SCALE_H

#include "BaseGeometry.h"

class G4GenericMessenger;

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

    G4double gas_density_;
    G4double active_diam_, active_length_;
    G4double fcage_thickn_, ics_thickn_, vessel_thickn_;
    G4double endcap_hollow_;
    G4double water_thickn_;

    G4double vessel_diam_, vessel_length_; 
    G4double ics_diam_, ics_length_;
    G4double fcage_diam_, fcage_length_;
    G4double gas_diam_, gas_length_;

    G4double specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_;

    //CylinderPointSampler* rnd_;
    CylinderPointSampler* active_gen_;
    CylinderPointSampler* field_cage_gen_;
    CylinderPointSampler* ics_gen_;
    CylinderPointSampler* vessel_gen_;
    CylinderPointSampler* readout_plane_gen_;
    CylinderPointSampler* outer_plane_gen_;
  };

} // namespace nexus

#endif
