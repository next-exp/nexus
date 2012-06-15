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

#ifndef __NEXT1_EL_DBO__
#define __NEXT1_EL_DBO__

#include "BaseGeometry.h"


namespace nexus {

  class Next1ELDBO: public BaseGeometry
  {
  public:
    /// Constructor
    Next1ELDBO(G4int rows, G4int columns);
    /// Destructor
    ~Next1ELDBO();
    G4ThreeVector GetDimensions();
    
  private:
    void DefineGeometry(G4int rows, G4int columns);
    G4ThreeVector _dimensions;
  };
  
} // end namespace nexus

#endif
