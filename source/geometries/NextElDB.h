// -----------------------------------------------------------------------------
// nexus | NextElDB.h
//
// Geometry of the NEXT-DEMO SiPM board.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_EL_DB_H
#define NEXT_EL_DB_H

#include "GeometryBase.h"
#include <vector>

namespace nexus {

  class SiPM11;

  class NextElDB: public GeometryBase
  {
  public:
    /// Constructor
    NextElDB(G4int rows, G4int columns);

    /// Destructor
    ~NextElDB();

    G4ThreeVector GetDimensions();

    std::vector<std::pair<int, G4ThreeVector> > GetPositions();

    /// Builder
    void Construct();


  private:
    G4int rows_, columns_;
    G4ThreeVector dimensions_;
    std::vector<std::pair<int, G4ThreeVector> > positions_;

    SiPM11* siPM_;

  };

} // end namespace nexus

#endif
