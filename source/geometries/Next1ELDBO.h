// ----------------------------------------------------------------------------
// nexus | Next1ELDBO.h
//
// Dice boards of the NEXT-DEMO detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT1_EL_DBO
#define NEXT1_EL_DBO

#include "BaseGeometry.h"
#include <vector>


namespace nexus {

  class Next1ELDBO: public BaseGeometry
  {
  public:
    /// Constructor
    Next1ELDBO(G4int rows, G4int columns);
    /// Destructor
    ~Next1ELDBO();
    G4ThreeVector GetDimensions();
    /// Method to retrieve the position of SiPM inside the daughter board
    std::vector<std::pair<int, G4ThreeVector> > GetPositions();

  private:
    void DefineGeometry(G4int rows, G4int columns);
    G4ThreeVector dimensions_;
    std::vector<std::pair<int, G4ThreeVector> > positions_;
  };

} // end namespace nexus

#endif
