// ----------------------------------------------------------------------------
// nexus | CylindricChamber.h
//
// General-purpose cylindric chamber.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef CYLINDRIC_CHAMBER_H
#define CYLINDRIC_CHAMBER_H

#include "BaseGeometry.h"

class G4GenericMessenger;


namespace nexus {

  class CylindricChamber: public BaseGeometry
  {
  public:
    /// Constructor
    CylindricChamber();
    /// Destructor
    ~CylindricChamber();

    /// Return vertex within region <region> of the chamber
    virtual G4ThreeVector GenerateVertex(const G4String& region) const;

    virtual void Construct();

  private:
    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;
  };

} // end namespace nexus

#endif
