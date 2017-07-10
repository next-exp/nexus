#ifndef TH228_SOURCE_H
#define TH228_SOURCE_H

#include "DiskSource.h"

namespace nexus {

  class Th228Source: public DiskSource {
  public:
    /// Constructor
    Th228Source();

    /// Destructor
    ~Th228Source();

    void Construct();
   
    /* G4double GetSourceDiameter();  */
    /* G4double GetSourceThickness(); */
    
    /* G4double GetSupportDiameter(); */
    /* G4double GetSupportThickness(); */

  private:
    
    /* // Dimension of the source itself */
    /* G4double _source_diam; */
    /* G4double _source_thick; */

    /* // Dimension of the whole support */
    /* G4double _support_diam; */
    /* G4double _support_thick; */
    

  };
}
#endif
