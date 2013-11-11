// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <paola.ferrario@ific.uv.es>
///  \date     1 Mar 2012
///  \version  $Id$
///
///  Copyright (c) 2012 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100_FIELDCAGE__
#define __NEXT100_FIELDCAGE__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"


class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  /// This is a geometry formed by the reflector tube and 
  /// TPB layer if needed

  class Next100FieldCage: public BaseGeometry
  {

  public:
    /// Constructor
    Next100FieldCage();
    /// Destructor
    ~Next100FieldCage();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    ///
    G4ThreeVector GetPosition() const;

    /// Builder
    void Construct();


  private:
    // Pointers to materials definition
    G4Material* _hdpe;


    // Dimensions
    G4double _tube_diam, _tube_length, _tube_thickn, _tube_zpos;
    G4double _refl_thickn;

   
    // Visibility of the shielding
    G4bool _visibility;


    // Vertex generators
    CylinderPointSampler* _body_gen;


    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 


  };

} //end namespace nexus
#endif
