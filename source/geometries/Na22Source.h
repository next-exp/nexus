// ----------------------------------------------------------------------------
// nexus | Na22Source.h
//
// Na-22 calibration specific source with plastic support used at LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NA22_SOURCE_H
#define NA22_SOURCE_H

#include "DiskSource.h"

namespace nexus {

  class Na22Source: public DiskSource {
  public:
    /// Constructor
    Na22Source();

    /// Destructor
    ~Na22Source();

    void Construct();
   
    /* G4double GetSourceDiameter();  */
    /* G4double GetSourceThickness(); */
    
    /* G4double GetSupportDiameter(); */
    /* G4double GetSupportThickness(); */

  private:
    
    /* // Dimension of the source itself */
    /* G4double source_diam_; */
    /* G4double source_thick_; */

    /* // Dimension of the whole support */
    /* G4double support_diam_; */
    /* G4double support_thick_; */
    

  };
}
#endif
