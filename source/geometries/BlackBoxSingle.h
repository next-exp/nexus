// ----------------------------------------------------------------------------
//  nexus | BlackBoxSingle.h
//  SensL SiPM in a black box.
//  Author:  Miryam Martínez Vara
// ----------------------------------------------------------------------------

#ifndef BLACK_BOX_SINGLE_H
#define BLACK_BOX_SINGLE_H

#include "BaseGeometry.h"
#include "SiPMSensl.h"

class G4Material;
class G4OpticalSurface;
class G4GenericMessenger;
//namespace nexus { class SpherePointSampler; } //PUEDE QUE QUITE ESTO


namespace nexus {

  class BlackBoxSingle: public BaseGeometry
  {
  public:
    /// Constructor
    BlackBoxSingle();
    /// Destructor
    ~BlackBoxSingle();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;


    void Construct();

  //private:
    //G4OpticalSurface* GetPhotOptSurf();

  private:
    // Dimensions
    G4double _world_z;
    G4double _world_xy;
    // SiPM
    SiPMSensl* sipm_;
    G4double sipm_x_pos_;
    G4double sipm_y_pos_;

    G4bool _visibility;

    //Messenger for configuration parameters
    G4GenericMessenger* _msg;

    /// Specific vertex for AD_HOC region
    G4double _specific_vertex_X;
    G4double _specific_vertex_Y;
    G4double _specific_vertex_Z;
    G4double _sipm_z_pos;

  };

} // end namespace nexus

#endif
