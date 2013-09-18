// ----------------------------------------------------------------------------
///  \file   NextNewKDB.h
///  \brief  
///
///  \author  <miquel.nebot@ific.uv.es>
///  \date    18 Sept 2013
///  \version $Id: 
//
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT_NEW_KDB__
#define __NEXT_NEW_KDB__

#include "SiPM11.h"
#include "BaseGeometry.h"
#include <vector>

namespace nexus {

  class NextNewKDB: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewKDB(G4int rows, G4int columns);
    /// Destructor
    ~NextNewKDB();

    G4ThreeVector GetDimensions();
    std::vector<std::pair<int, G4ThreeVector> > GetPositions();
    /// Builder
    void Construct();

  private:
    G4int _rows, _columns;
    G4ThreeVector _dimensions;
    std::vector<std::pair<int, G4ThreeVector> > _positions;

    SiPM11* _siPM;
  };
} // end namespace nexus
#endif
    
