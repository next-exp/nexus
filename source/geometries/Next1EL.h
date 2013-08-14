// ----------------------------------------------------------------------------
///  \file   Next1EL.h
///  \brief  Geometry of the NEXT-1 EL detector.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     16 Feb 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT1_EL__
#define __NEXT1_EL__

#include "BaseGeometry.h"

#include <iostream>
#include <fstream>

namespace nexus {

  class HexagonNavigator;

  
  /// Geometry of the NEXT-1 EL protoype
    
  class Next1EL: public BaseGeometry
  {
  public:
    /// Constructor
    Next1EL();
    /// Destructor
    ~Next1EL();

    /// Returns a vertex in a region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:

    void DefineGeometry();

  private:
    G4double _radius;
    G4double _length;

    HexagonNavigator* _hexnav;

    G4int _PMTID;
    G4int _SiPMID;

    std::ofstream _PMTTable;
    std::ofstream _SiPMTable;
  };


} // end namespace nexus

#endif
