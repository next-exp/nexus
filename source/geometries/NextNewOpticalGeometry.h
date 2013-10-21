///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     18 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_OPTICALGEOMETRY__
#define __NEXTNEW_OPTICALGEOMETRY__

#include "BaseGeometry.h"
namespace nexus { class NextNewVessel; }
namespace nexus { class NextNewInnerElements; }
namespace nexus { class NextNewIcs; }
//namespace nexus { class Enclosure; }
//namespace nexus { class NextNewEnergyPlane; }
//namespace nexus { class NextNewTrackingPlane; }
//namespace nexus { class NextNewKDB; }
//namespace nexus { class SiPM11;}
//namespace nexus { class NextNewFieldCage; } 

namespace nexus {

  class NextNewOpticalGeometry: public BaseGeometry
  {
    /// This geometry is a container of those parts needed 
    /// for light generation and collection, namely the field cage and the sensors

  public:
    ///Constructor
    NextNewOpticalGeometry();
    ///Destructor
    ~NextNewOpticalGeometry();
    
    /// Returns a vertex in a region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;
    /// Builder
    void Construct();

  private:     
    G4double _pressure;
    NextNewVessel* _vessel;
    NextNewInnerElements* _inner_elements;
    NextNewIcs* _ics;
    //Enclosure* _enclosure;
    //NextNewEnergyPlane*    _energy_plane;
    //NextNewKDB* _kdb;
    //SiPM11* _sipm;
    //NextNewTrackingPlane* _tracking_plane;
    //NextNewFieldCage* _field_cage;

  };

} // end namespace nexus
#endif
