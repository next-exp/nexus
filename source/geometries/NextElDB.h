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

#ifndef __NEXT_EL_DB__
#define __NEXT_EL_DB__

#include "BaseGeometry.h"


namespace nexus {

  class NextElDB: public BaseGeometry
  {
  public:
    /// Constructor
    NextElDB(G4int rows, G4int columns);

    /// Destructor
    ~NextElDB();

    G4ThreeVector GetDimensions();
    
  private:
    void DefineGeometry(G4int rows, G4int columns);
    G4ThreeVector _dimensions;

  };
  
} // end namespace nexus

#endif
