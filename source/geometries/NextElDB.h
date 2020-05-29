// ----------------------------------------------------------------------------
///  \file   NextElDB.h
///  \brief
///
///  \author  <justo.martin-albo@ific.uv.es>, <jmunoz@ific.uv.es>
///  \date    2 Nov 2010
///  \version $Id: Next1DBO.h 3279 2010-11-17 12:12:03Z jmalbos $
//
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT_EL_DB
#define NEXT_EL_DB

#include "SiPM11.h"
#include "BaseGeometry.h"
#include <vector>
// -----------------------------------------------------------------------------
// nexus | NextElDB.h
// -----------------------------------------------------------------------------
// nexus | NextElDB.cc
//
// Geometry of the NEXT-DEMO SiPM board.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

namespace nexus {

  class NextElDB: public BaseGeometry
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
