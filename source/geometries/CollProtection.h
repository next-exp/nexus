// ----------------------------------------------------------------------------
// nexus | CollProtection.h
//
// Aluminum box used to protect calibration sources at LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef COLL_PROTECTION_H
#define COLL_PROTECTION_H

#include "BaseGeometry.h"

namespace nexus {
  class CollProtection : public BaseGeometry {

  public:
    ///Constructor
    CollProtection();

    ///Destructor
    ~CollProtection();

    void Construct();
    G4double GetAxisCentre();

  private:
    G4double axis_centre_;
  };
}

#endif
