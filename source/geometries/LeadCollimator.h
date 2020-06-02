// ----------------------------------------------------------------------------
// nexus | LeadCollimator.h
//
// Lead colllimator used at LSC to screen source from lateral port.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef LEAD_COLL_H
#define LEAD_COLL_H

#include "BaseGeometry.h"

namespace nexus {

  class LeadCollimator : public BaseGeometry
  {

  public:
    ///Constructor
    LeadCollimator();

    ///Destructor
    ~LeadCollimator();

    void Construct();

    G4double GetAxisCentre();
    G4double GetLength();

  private:
    G4double axis_centre_;
    G4double length_;
  };

}

#endif
