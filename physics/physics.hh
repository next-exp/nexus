// NEXT simulation: physics.hh
//

#ifndef physics_h
#define physics_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4UnitsTable.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

#include <bhep/engine.h>
#include <bhep/sreader.h>

//using namespace bhep;


class physics: public G4VUserPhysicsList , public bhep::engine
{
public:
  physics();
  ~physics();
  
protected:
  // Construct particle and physics
  void ConstructParticle();
  void ConstructProcess();
  
  void SetCuts();
  
   
protected:
  // these methods Construct particles 
  void ConstructBosons();
  void ConstructLeptons();
  void ConstructMesons();
  void ConstructBaryons();
  
protected:
  // these methods Construct physics processes and register them
  void ConstructGeneral();
  void ConstructEM();
  
private:
  int e_loss_;
  int mult_scat_;
  double bulk_cut_;

};
#endif
