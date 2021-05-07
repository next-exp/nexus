// ----------------------------------------------------------------------------
// nexus | CollSupport.h
//
// Support for the collimator used with calibration sources at LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef COLL_SUPPORT_H
#define COLL_SUPPORT_H

#include "GeometryBase.h"

namespace nexus {
  class CollSupport : public GeometryBase {

  public:
    ///Constructor
    CollSupport();

    ///Destructor
    ~CollSupport();

    void Construct();
    G4double GetAxisCentre();
    G4double GetYDisplacement();

  private:
    G4double axis_centre_;
    G4double y_displ_;
  };
}

#endif
