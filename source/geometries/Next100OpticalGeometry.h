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
#include "BaseGeometry.h"

class G4GenericMessenger;


namespace nexus {

  class Next100InnerElements;

  class Next100OpticalGeometry : public BaseGeometry
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

    G4double pressure_;
    G4double temperature_;
    G4double sc_yield_;
    G4double e_lifetime_;

    // Vertex decided by user
    G4double specific_vertex_X_;
    G4double specific_vertex_Y_;
    G4double specific_vertex_Z_;

    G4String gas_;

    Next100InnerElements* inner_elements_;

    // Relative position of the gate in its mother volume
    G4double gate_zpos_in_gas_;

  };

} // end namespace nexus

#endif
