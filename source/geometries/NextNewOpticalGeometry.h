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

class G4GenericMessenger;
namespace nexus { class NextNewInnerElements; }

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
    G4GenericMessenger* _msg;
    G4double _pressure;
    G4double _temperature;
    G4double _sc_yield;
    G4String _gas;
    NextNewInnerElements* _inner_elements;

    // Rotation around Y and displacement of the whole geometry in the g4 system of reference
    G4ThreeVector _displ;
    G4double _rot_angle;
   
   
  };

} // end namespace nexus
#endif
