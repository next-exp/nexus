// ----------------------------------------------------------------------------
// nexus | SurroundingAir.h
//
// Volume of air to be placed inside the LSC lead castle.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SURROUNDING_AIR_H
#define SURROUNDING_AIR_H

#include "BaseGeometry.h"

namespace nexus {

  class SurroundingAir : public BaseGeometry
  {
  public:
    ///Constructor
    SurroundingAir();

    ///Destructor
    ~SurroundingAir();

    void Construct();

    
  private:
    
  };
}

#endif
