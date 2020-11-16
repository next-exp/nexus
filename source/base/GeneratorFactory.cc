// ----------------------------------------------------------------------------
// nexus | GeneratorFactory.cc
//
// This class instantiates the generator of the simulation that the user
// specifies via configuration parameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "GeneratorFactory.h"

#include <G4GenericMessenger.hh>
#include <G4VPrimaryGenerator.hh>

using namespace nexus;


GeneratorFactory::GeneratorFactory(): msg_(0)
{
  msg_ = new G4GenericMessenger(this, "/Generator/");
  msg_->DeclareProperty("RegisterGenerator", name_, "");
}


GeneratorFactory::~GeneratorFactory()
{
  delete msg_;
}


//////////////////////////////////////////////////////////////////////

#include "SingleParticleGenerator.h"
#include "Decay0Interface.h"
#include "IonGenerator.h"
#include "Na22Generator.h"
#include "Kr83mGenerator.h"
#include "SingleParticle2PiGenerator.h"
#include "MuonGenerator.h"
#include "MuonAngleGenerator.h"
#include "ELTableGenerator.h"
#include "ScintillationGenerator.h"
#include "ElecPositronPairGenerator.h"
#include "SingleParticleAngle.h"


G4VPrimaryGenerator* GeneratorFactory::CreateGenerator() const
{
  G4VPrimaryGenerator* p = 0;

  if      (name_ == "SINGLE_PARTICLE") p = new SingleParticleGenerator();

  else if (name_ == "DECAY0")          p = new Decay0Interface();

  else if (name_ == "ION")             p = new IonGenerator();

  else if (name_ == "NA22")            p = new Na22Generator();

  else if (name_ == "Kr83m")           p = new Kr83mGenerator();

  else if (name_ == "2PI")             p = new SingleParticle2PiGenerator();

  else if (name_ == "MUON")            p = new MuonGenerator();

  else if (name_ == "LAB_MUON")        p = new MuonAngleGenerator();

  else if (name_ == "EL_TABLE")        p = new ELTableGenerator();

  else if (name_ == "SCINTILLATION")   p = new ScintillationGenerator();

  else if (name_ == "E+E-PAIR")        p = new ElecPositronPairGenerator();

  else if (name_ == "SINGLE_PARTICLE_ANG") p = new SingleParticleAngle();

  else {
    G4String err = "The user specified an unknown generator: " + name_;
    G4Exception("[GeneratorFactory]", "CreateGenerator()",
        FatalException, err);
  }

  return p;
}
