// NEXT simulation: generator.hh
// 

#ifndef generator_h 
#define generator_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleMomentum.hh"
#include "globals.hh"
#include "Randomize.hh"

#include <bhep/bhep_svc.h>
//#include <bhep/error.h>
#include <bhep/sreader.h>
#include <bhep/engine.h>

#include <iostream>
#include <fstream>


class G4ParticleGun;
class G4Event;


using namespace std;

class generator : public G4VUserPrimaryGeneratorAction, public bhep::engine
{
public:
  
  generator(G4double, G4double, G4String, G4String);
  generator(G4String);
  ~generator();
  
public:
  void GeneratePrimaries(G4Event*);
  
private:
  
  G4String idata_type_;
  
  bhep::reader_gz r_;
  
  G4ParticleGun* particle_gun_;
  
  G4double E_min_;
  G4double E_max_;

  G4double CHAMBER_length_;
  G4double CHAMBER_height_;
  G4double CHAMBER_width_;

private:
  
  void read_param(string);
  void read_event(G4Event*);
  void gen_random_position();
  void gen_random_direction();
  void gen_random_energy();

};
#endif
