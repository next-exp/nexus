// -----------------------------------------------------------------------------
//  nexus | Next100SiPM.h
//
//  Geometry of the Hamamatsu MPPC S13372-1350TE, the model of
//  silicon photomultiplier (SiPM) used in the NEXT-100 detector.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT100_SIPM_H
#define NEXT100_SIPM_H

#include "BaseGeometry.h"

namespace nexus {

  class Next100SiPM: public BaseGeometry
  {
  public:
    Next100SiPM();
    ~Next100SiPM();
    void Construct() override;
  };

}

#endif
