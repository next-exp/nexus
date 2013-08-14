// ----------------------------------------------------------------------------
///  \file   SiPM11.h
///  \brief  
///
///  \author   L. Serra (luis.serra@ific.uv.es)
///  \date     2 March 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __SILICON_PM_11__
#define __SILICON_PM_11__

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

namespace nexus {

  
  class SiPM11: public BaseGeometry
  {
  public:
    /// Constructor
    SiPM11();
    /// Destructor
    ~SiPM11();
    
    G4ThreeVector GetDimensions();
    
  private:
    void BuildGeometry();
    
  private:
    G4ThreeVector _dimensions; ///< external dimensions of the SiPM11
  };

} // end namespace nexus

#endif
