// ----------------------------------------------------------------------------
///  \file   SiPM11.h
///  \brief  Geometry of a 1x1 mm2 SiPM
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     2 March 2010
///  \version  $Id$
///
///  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef SILICON_PM_11_H
#define SILICON_PM_11_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

namespace nexus {


  /// Geometry of the Hamamatsu surface-mounted 1x1 mm2 MPPC (SiPM)
  
  class SiPM11: public BaseGeometry
  {
  public:
    /// Constructor
    SiPM11();
    /// Destructor
    ~SiPM11();
    
    /// Return dimensions of the SiPM
    G4ThreeVector GetDimensions() const;
    
    /// Invoke this method to build the volumes of the geometry
    void Construct();
    
  private:
    G4ThreeVector _dimensions; ///< external dimensions of the SiPM11
  };


} // end namespace nexus

#endif
