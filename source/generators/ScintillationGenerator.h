// ----------------------------------------------------------------------------
///  \file   ScintillationGenerator.h
///  \brief  Primary generator for scintillation in HPXe
///  
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     27 Mar 2009
///  \version  $Id: ScintillationGenerator.h 9216 2013-09-05 12:57:42Z paola $
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __SCINTILLATIONGENERATOR__
#define __SCINTILLATIONGENERATOR__

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

namespace nexus {

  class BaseGeometry;


  /// Primary generator (concrete class of G4VPrimaryGenerator) for events 
  /// consisting of a single particle. The user must specify via configuration
  /// parameters the particle type, a kinetic energy interval (a random
  /// value with random .
  
  class ScintillationGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    ScintillationGenerator();
    /// Destructor
    ~ScintillationGenerator();
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);
    void SetXposition(double);
    void SetYposition(double);
    void SetZposition(double);
    void SetEnergy(double);
    G4double GetEnergy();
    
  private:

    void SetParticleDefinition(G4String);

    G4GenericMessenger* _msg;
    G4ParticleDefinition* _particle_definition;
    const BaseGeometry* _geom; ///< Pointer to the detector geometry

    G4double _position_X;
    G4double _position_Y;
    G4double _position_Z;
    G4double _energy;
    G4int    _nphotons;
   

  };

} // end namespace nexus

#endif // __SCINTILLATIONGENERATOR__
