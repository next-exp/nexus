// ----------------------------------------------------------------------------
///  \file   SiPMpetTPB.h
///  \brief  Geometry of a 1x1 mm2 SiPM
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     2 March 2010
///  \version  $Id$
///
///  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef SIPM_pet_TPB_H
#define SIPM_pet_TPB_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {


  /// Geometry of  3x3 mm2 active surface SiPMs
  
  class SiPMpetTPB: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMpetTPB();
    /// Destructor
    ~SiPMpetTPB();
    
    /// Return dimensions of the SiPM
    //G4ThreeVector GetDimensions() const;
    
    /// Invoke this method to build the volumes of the geometry
    void Construct();
    
  private:
    //G4ThreeVector _dimensions; ///< external dimensions of the SiPMpetTPB

    // Visibility of the tracking plane
    G4bool _visibility;
    
    // Optical properties to be used for epoxy
    G4double _refr_index;

    G4double _decay_time; ///< decay time of TPB

    G4bool _phys;

    G4double _time_binning;

     // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 



  };


} // end namespace nexus

#endif
