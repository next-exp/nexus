// ----------------------------------------------------------------------------
///  \file   Next100OpticalGeometry.h
///  \brief  Geometry of the inner elements of NEXT-100.
///
///  \author   <paola.ferrario@dipc.org>
///  \date     30 Dec 2019
///
///  Copyright (c) 2019 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_OPT_GEO_H
#define NEXT100_OPT_GEO_H

#include <G4ThreeVector.hh>
#include "BaseGeometry.h"

class G4GenericMessenger;


namespace nexus {

  class Next100InnerElements;

  /// This is a geometry placer that encloses from the FIELD CAGE to inside
  /// covering the ACTIVE region, EL gap and its grids, the cathode grid ...

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
    G4GenericMessenger* _msg;

    G4double _pressure;
    G4double _temperature;
    G4double _sc_yield;
    G4double _e_lifetime;

    // Vertex decided by user
    G4double _specific_vertex_X;
    G4double _specific_vertex_Y;
    G4double _specific_vertex_Z;

    G4String _gas;

    Next100InnerElements* _inner_elements;

    // Relative position of the gate in its mother volume
    G4double _gate_zpos_in_gas;

  };

} // end namespace nexus

#endif
