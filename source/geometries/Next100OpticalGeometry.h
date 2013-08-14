///
///  \author   <paola.ferrario@ific.uv.es>
///  \date     1 Mar 2012
///  \version  $Id$
///
///  Copyright (c) 2012 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100_OPTICALGEOMETRY__
#define __NEXT100_OPTICALGEOMETRY__

#include "BaseGeometry.h"
#include "Next100InnerElements.h"

namespace nexus {

  class Next100OpticalGeometry: public BaseGeometry
  {
    /// This geometry is a container of those parts needed 
    /// for light generation and collection, namely the field cage and the sensors

  public:
    ///Constructor
    Next100OpticalGeometry();
    ///Destructor
    ~Next100OpticalGeometry();

    /// Returns a vertex in a region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:

    void ReadParameters();
    
    G4double _pressure;

    Next100InnerElements* _inner_elements;

  };

} // end namespace nexus
#endif
