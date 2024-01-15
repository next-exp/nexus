// ----------------------------------------------------------------------------
// nexus | Next100OpticalGeometry.h
//
// This class builds a simplified version of the NEXT-100 geometry, where
// only the inner elements are instantiated.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_OPT_GEO_H
#define NEXT100_OPT_GEO_H

#include <G4ThreeVector.hh>
#include "GeometryBase.h"

class G4GenericMessenger;


namespace nexus {

  class Next100InnerElements;

  class Next100OpticalGeometry : public GeometryBase
  {

  public:
    ///Constructor
    Next100OpticalGeometry();

    /// Destructor
    ~Next100OpticalGeometry();

    /// Returns a vertex in a region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    const G4double grid_thickness_;
    const G4double gate_tracking_plane_distance_, gate_sapphire_wdw_distance_;
    G4double pressure_;
    G4double temperature_;
    G4double sc_yield_;
    G4double e_lifetime_;
    G4double fc_displ_x_, fc_displ_y_;
    G4ThreeVector coord_origin_;

    // Vertex decided by user
    G4ThreeVector specific_vertex_;

    G4String gas_;

    Next100InnerElements* inner_elements_;

  };

} // end namespace nexus

#endif
