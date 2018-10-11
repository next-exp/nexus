// ----------------------------------------------------------------------------
/// 
///
///  Copyright (c) 2018 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef EXTRA_VESSEL_H
#define EXTRA_VESSEL_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4OpticalSurface;
class G4GenericMessenger;

namespace nexus {

  class CylinderPointSampler;


  /// Geometry model for the Hamamatsu R11410-10 photomultiplier
  
  class ExtraVessel: public BaseGeometry
  {
  public:
    /// Constructor
    ExtraVessel();

    /// Destructor
    ~ExtraVessel();

    G4ThreeVector GetRelPosition();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    // Builder
    void Construct();

  private:
    // Dimensions
    G4double _diameter, _thickness;

    // Vertex generators
    CylinderPointSampler* _generic_gen;
    
    G4bool _visibility;

    //Messenger for configuration parameters
    G4GenericMessenger* _msg;

  };


} // end namespace nexus

#endif
