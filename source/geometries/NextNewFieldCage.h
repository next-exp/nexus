// ----------------------------------------------------------------------------
///  \file     NextNewFieldCage.h  
///  \brief    This is a geometry formed by the reflector tube and 
///            TPB layer if needed
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     12 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_FIELDCAGE__
#define __NEXTNEW_FIELDCAGE__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;

namespace nexus {

  class NextNewFieldCage: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewFieldCage();
    /// Destructor
    ~NextNewFieldCage();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;
    ///
    G4ThreeVector GetPosition() const;
    /// Builder
    void Construct();

  private:
    // Pointers to materials definition
    G4Material* _hdpe;//High density polyethylene
    // Dimensions
    G4double _tube_in_rad, _tube_length, _tube_thickness, _tube_z_pos;
    G4double _reflector_thickness;
    // Visibility of the shielding
    G4bool _visibility;
    // Vertex generators
    CylinderPointSampler* _body_gen;
    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 
  };

} //end namespace nexus
#endif
