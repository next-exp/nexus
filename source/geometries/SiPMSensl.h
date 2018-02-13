// ----------------------------------------------------------------------------
///  \file   SiPMSensl.h
///  \brief  Geometry of a 1x1 mm2 SiPM
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     2 March 2010
///  \version  $Id$
///
///  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef SILICON_PM_SENSL_H
#define SILICON_PM_SENSL_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {


  /// Geometry of the Hamamatsu surface-mounted 1x1 mm2 MPPC (SiPM)
  
  class SiPMSensl: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMSensl();
    /// Destructor
    ~SiPMSensl();
    
    /// Return dimensions of the SiPM
    G4ThreeVector GetDimensions() const;
    
    /// Invoke this method to build the volumes of the geometry
    void Construct();
    
  private:
    G4ThreeVector _dimensions; ///< external dimensions of the SiPMSensl

    // Visibility of the tracking plane
    G4bool _visibility;

    // Time binning of sensors
    G4double _binning;

     // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };


} // end namespace nexus

#endif
