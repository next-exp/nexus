// ----------------------------------------------------------------------------
// nexus | NextTonScale.h
//
// Geometry of a possible tonne-scale NEXT detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT_TON_SCALE_H
#define NEXT_TON_SCALE_H

#include "GeometryBase.h"

class G4GenericMessenger;
class G4Material;

namespace nexus { class BoxPointSamplerLegacy; }
namespace nexus { class CylinderPointSamplerLegacy; }


namespace nexus {

  class NextTonScale: public GeometryBase
  {
  public:
    // Constructor
    NextTonScale();
    // Destructor
    virtual ~NextTonScale();
    //
    virtual void Construct();
    //
    virtual G4ThreeVector GenerateVertex(const G4String&) const;

  private:
    //
    G4LogicalVolume* ConstructWaterTank(G4LogicalVolume*);
    G4LogicalVolume* ConstructVesselAndICS(G4LogicalVolume*);
    G4LogicalVolume* ConstructFieldCageAndReadout(G4LogicalVolume*);
    //
    void DefineConfigurationParameters();
    //
    void DefineGas();

  private:
    G4GenericMessenger* msg_; // Messenger for configuration parameters

    G4Material* xenon_gas_;

    G4String gas_;
    G4double gas_pressure_, gas_temperature_;
    G4double detector_diam_, detector_length_;
    G4double tank_size_, tank_thickn_, water_thickn_;
    G4double vessel_thickn_, ics_thickn_, endcap_hollow_;
    G4double fcage_thickn_, active_diam_, active_length_;
    G4double cathode_thickn_, anode_thickn_, readout_gap_;
    G4double xe_perc_, helium_mass_num_;

    G4bool tank_vis_, vessel_vis_, ics_vis_,
           fcage_vis_, cathode_vis_, anode_vis_, readout_vis_;

    G4ThreeVector specific_vertex_;

    CylinderPointSamplerLegacy* active_gen_;
    CylinderPointSamplerLegacy* field_cage_gen_;
    CylinderPointSamplerLegacy* cathode_gen_;
    CylinderPointSamplerLegacy* ics_gen_;
    CylinderPointSamplerLegacy* vessel_gen_;
    CylinderPointSamplerLegacy* readout_plane_gen_;
    CylinderPointSamplerLegacy* outer_plane_gen_;
    CylinderPointSamplerLegacy* external_gen_;

    BoxPointSamplerLegacy* muon_gen_;
  };

} // namespace nexus

#endif
