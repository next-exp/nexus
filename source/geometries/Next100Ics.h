// ----------------------------------------------------------------------------
// nexus | Next100Ics.h
//
// Inner copper shielding of the NEXT-100 detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_ICS_H
#define NEXT100_ICS_H

#include "GeometryBase.h"

#include <G4Navigator.hh>

class G4GenericMessenger;


namespace nexus {

  class CylinderPointSampler;

  class Next100Ics: public GeometryBase
  {
  public:
    /// Constructor
    Next100Ics(G4double ics_ep_lip_width);

    /// Destructor
    ~Next100Ics();

    /// Sets the Logical Volume where ICS will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    void SetELtoTPdistance(G4double);
    void SetELtoSapphireWDWdistance(G4double);
    void SetPortZpositions(G4double port_positions[]);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();

  private:
    // Mother Logical Volume of the ICS
    G4LogicalVolume* mother_logic_;

    // Dimensions
    const G4double in_rad_, thickness_;
    const G4double ics_ep_lip_width_;
    G4double gate_tp_distance_, gate_sapphire_wdw_dist_;
    G4double port_z_1a_, port_z_2a_, port_z_1b_, port_z_2b_;

    // Visibility of the ICS
    G4bool visibility_;

    // Vertex generator
    CylinderPointSampler* ics_gen_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

  inline void Next100Ics::SetELtoTPdistance(G4double distance){
    gate_tp_distance_ = distance;
  }

  inline void Next100Ics::SetELtoSapphireWDWdistance(G4double distance){
    gate_sapphire_wdw_dist_ = distance;
  }

} // end namespace nexus

#endif
