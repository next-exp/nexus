// ----------------------------------------------------------------------------
///  \file   ElecPositronPair.h
///  \brief  Primary generator for single-particle events.
///  
///  \author   P Ferrario <paolafer@ific.uv.es>    
///  \date     2 Dic 2015
///  \version  $Id$
///
///  Copyright (c) 2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __ELEC_POSITRON__
#define __ELEC_POSITRON__

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
  /// Particle energy is generated with flat random probability
  /// between E_min and E_max.
  
  class ElecPositronPair: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    ElecPositronPair();
    /// Destructor
    ~ElecPositronPair();
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:

    /// Generate a random kinetic energy with flat probability in 
    //  the interval [energy_min, energy_max].
    G4double RandomEnergy(G4double emin, G4double emax) const;
    
  private:
    G4GenericMessenger* _msg;
  
    G4ParticleDefinition* _particle_definition;

    G4double _energy_min; ///< Minimum kinetic energy particle 1
    G4double _energy_max; ///< Maximum kinetic energy particle 1

    const BaseGeometry* _geom; ///< Pointer to the detector geometry

    G4String _region;
    
  };

} // end namespace nexus

#endif
