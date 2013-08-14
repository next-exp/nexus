// ----------------------------------------------------------------------------
///  \file   PmtR7378A.h
///  \brief  Geometry model for the Hamamatsu R7378A PMT. 
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     17 Feb 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __PMT_R7378A__
#define __PMT_R7378A__

#include "BaseGeometry.h"

#include <G4ThreeVector.hh>


namespace nexus {

  /// Geometry model for the Hamamatsu R7378A photo-multiplier (PMT).
  
  class PmtR7378A: public BaseGeometry
  {
  public:
    /// Constructor
    PmtR7378A();
    /// Destructor
    ~PmtR7378A();

    G4ThreeVector GetDimensions() const;
    
  private:
    void BuildGeometry();

  private:
    G4ThreeVector _dimensions;
  };

} // end namespace nexus

#endif
