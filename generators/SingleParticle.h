// ----------------------------------------------------------------------------
///  \file   SingleParticle.h
///  \brief  Primary generator for single-particle events.
///  
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     27 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __SINGLE_PARTICLE__
#define __SINGLE_PARTICLE__

#include <G4VPrimaryGenerator.hh>

class G4Event;
class G4ParticleDefinition;


namespace nexus {

  /// Primary generator for events containing a single particle.
  /// Particle energy is generated with flat random probability
  /// between E_min and E_max.
  
  class SingleParticle: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    SingleParticle();
    /// Destructor
    ~SingleParticle() {}
    
    /// This method is invoked in class PrimaryGeneration at the 
    /// beginning of the event loop
    void GeneratePrimaryVertex(G4Event*);

  private:
    void Initialize();
    G4double GenerateRandomEnergy(G4double, G4double);
    
  private:
    G4int _charge;
    G4double _mass;
    G4double _energy_min; 
    G4double _energy_max;
    G4String _region;
    G4ParticleDefinition* _particle_definition;
  };

} // end namespace nexus

#endif
