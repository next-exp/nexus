//
//  SingleParticle.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 27 Mar 2009
//     Updated:    Mar 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __SINGLE_PARTICLE__
#define __SINGLE_PARTICLE__

#include <G4VPrimaryGenerator.hh>
//#include <globals.hh>

class G4Event;
class G4ParticleDefinition;
namespace bhep {class gstore;}


namespace nexus {

  class SingleParticle: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    SingleParticle();
    /// Destructor
    ~SingleParticle() {}
    
    void GeneratePrimaryVertex(G4Event*);

  private:
    void Initialize();
    G4ThreeVector GenerateRandomDirection();
    G4double GenerateRandomEnergy(G4double, G4double);
    
  private:
    const bhep::gstore& _input;
    G4ParticleDefinition* _particle_definition;
    G4double _mass;
    G4int _charge;
    G4double _energy_min, _energy_max;
  };

} // end namespace nexus

#endif
