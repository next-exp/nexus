// ----------------------------------------------------------------------------
// nexus | NaIScintillator.h
//
// Sodium Iodide scintillator, used with Na22 calibration source.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NAI_SCINT_H
#define NAI_SCINT_H

#include "BaseGeometry.h"

class G4GenericMessenger;

namespace nexus {

  class NaIScintillator : public BaseGeometry
  {
  public:
    ///Constructor
    NaIScintillator();

    ///Destructor
    ~NaIScintillator();

    void Construct();

    // Retrieve scintillator's length
    G4double GetLength();

  private:

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;
    G4bool visibility_;
    G4double length_;

  };
}

#endif
