// ----------------------------------------------------------------------------
///  \file   Test_Geometry.h
///  \brief  ...
///
///  \author   Miryam Martínez Vara <Miryam.Martinez@ific.uv.es>    
///  \date     7 May 2020
///  \version  $Id$
///
///  Copyright (c) 2020 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef TEST_GEOMETRY_H
#define TEST_GEOMETRY_H

#include "BaseGeometry.h"

class G4Material;
class G4OpticalSurface;
class G4GenericMessenger;
namespace nexus { class SpherePointSampler; } //PUEDE QUE QUITE ESTO


namespace nexus {
  
  /// Spherical chamber filled with xenon (liquid or gas)
  
  class Test_Geometry: public BaseGeometry
  {
  public:
    /// Constructor
    Test_Geometry();
    /// Destructor
    ~Test_Geometry();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;


    void Construct();

  private:
    G4OpticalSurface* GetPhotOptSurf();

  private:
    // Dimensions
    G4double _world_size;
    G4double _tpb_diam_outer; 
    G4double _tpb_diam_inner;
    G4double _tpb_length;
    G4double _pedot_diam_outer; 
    G4double _pedot_diam_inner;
    G4double _pedot_length ;
    G4double _sapphire_diam_outer; 
    G4double _sapphire_diam_inner;
    G4double _sapphire_length;

    // Gas
    G4String _gas;
    G4double _pressure, _temperature;
    G4int _helium_mass_num;
    G4double _xe_perc;

    G4bool _visibility;

    //Messenger for configuration parameters
    G4GenericMessenger* _msg;

    /// Specific vertex for AD_HOC region
    G4double _specific_vertex_X;
    G4double _specific_vertex_Y;
    G4double _specific_vertex_Z;

  };

} // end namespace nexus

#endif
