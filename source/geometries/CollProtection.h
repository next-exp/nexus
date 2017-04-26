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
    G4double _axis_centre;
  };
}

#endif
