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

#include "SingleParticle.h"
#include "Decay0Interface.h"
#include "IonGun.h"
#include "Na22Generation.h"
#include "Kr83mGeneration.h"
#include "SingleParticle2Pi.h"
#include "MuonGenerator.h"
#include "MuonAngleGenerator.h"
#include "ELTableGenerator.h"
#include "ScintillationGenerator.h"
#include "ElecPositronPair.h"


G4VPrimaryGenerator* GeneratorFactory::CreateGenerator() const
{
  G4VPrimaryGenerator* p = 0;

  if (name_ == "SINGLE_PARTICLE") p = new SingleParticle();

  else if (name_ == "DECAY0") p = new Decay0Interface();

  else if (name_ == "ION_GUN") p = new IonGun();

  else if (name_ == "NA22") p = new Na22Generation();

  else if (name_ == "Kr83m") p = new Kr83mGeneration();

  else if (name_ == "2PI") p = new SingleParticle2Pi();

  else if (name_ == "MUON_GENERATOR") p = new MuonGenerator();

  else if (name_ == "LABMUON_GENERATOR") p = new MuonAngleGenerator();

  else if (name_ == "EL_TABLE_GENERATOR") p = new ELTableGenerator();

  else if (name_ == "SCINTGENERATOR") p = new ScintillationGenerator();

  else if (name_ == "E+E-PAIR") p = new ElecPositronPair();

  else {
    G4String err = "The user specified an unknown generator: " + name_;
    G4Exception("CreateGenerator()", "[GeneratorFactory]",
        FatalException, err);
  }

  return p;
}
