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
    G4double _axis_centre;
    G4double _length;
  };
  
}

#endif
