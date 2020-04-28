// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 13 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "GeneratorFactory.h"

#include <G4GenericMessenger.hh>
#include <G4VPrimaryGenerator.hh>

using namespace nexus;


GeneratorFactory::GeneratorFactory(): _msg(0)
{
  _msg = new G4GenericMessenger(this, "/Generator/");
  _msg->DeclareProperty("RegisterGenerator", _name, "");
}


GeneratorFactory::~GeneratorFactory()
{
  delete _msg;
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
  
  if (_name == "SINGLE_PARTICLE") p = new SingleParticle();

  else if (_name == "DECAY0") p = new Decay0Interface();

  else if (_name == "ION_GUN") p = new IonGun();

  else if (_name == "NA22") p = new Na22Generation();

  else if (_name == "Kr83m") p = new Kr83mGeneration();

  else if (_name == "2PI") p = new SingleParticle2Pi();

  else if (_name == "MUON_GENERATOR") p = new MuonGenerator();

  else if (_name == "LABMUON_GENERATOR") p = new MuonAngleGenerator();

  else if (_name == "EL_TABLE_GENERATOR") p = new ELTableGenerator();

  else if (_name == "SCINTGENERATOR") p = new ScintillationGenerator();

  else if (_name == "E+E-PAIR") p = new ElecPositronPair();

  else {
    G4String err = "The user specified an unknown generator: " + _name;
    G4Exception("CreateGenerator()", "[GeneratorFactory]",
        FatalException, err);
  }

  return p;
}


