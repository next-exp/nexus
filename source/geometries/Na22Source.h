#ifndef NA22_SOURCE_H
#define NA22_SOURCE_H

#include "BaseGeometry.h"

namespace nexus {

  class Na22Source: public BaseGeometry {
  public:
    /// Constructor
    Na22Source();

    /// Destructor
    ~Na22Source();

    void Construct();
   
    G4double GetSourceDiameter(); 
    G4double GetSourceThickness();
    
    G4double GetSupportDiameter();
    G4double GetSupportThickness();

  private:
    
    // Dimension of the Na22 source itself
    G4double _source_diam;
    G4double _source_thick;

    // Dimension of the whole support
    G4double _support_diam;
    G4double _support_thick;
    

  };
}
#endif
