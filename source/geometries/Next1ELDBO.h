// ----------------------------------------------------------------------------
///  \file   Next1ELDBO.h
///  \brief  
///
///  \author  <justo.martin-albo@ific.uv.es>
///  \date    2 Nov 2010
///  \version $Id: Next1DBO.h 3279 2010-11-17 12:12:03Z jmalbos $
//
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef _NEXT1_EL_DBO___
#define _NEXT1_EL_DBO___

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
