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
  delete _p;
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
#include "NeutronGenerator.h"
#include "ELTableGenerator.h"
#include "ScintillationGenerator.h"
#include "ElecPositronPair.h"
#include "DoubleParticle.h"
#include "Back2backGammas.h"

G4VPrimaryGenerator* GeneratorFactory::CreateGenerator()
{
  //G4VPrimaryGenerator* p = 0;
  
  if (_name == "SINGLE_PARTICLE") _p = new SingleParticle();

  else if (_name == "DECAY0") _p = new Decay0Interface();

  else if (_name == "ION_GUN") _p = new IonGun();

  else if (_name == "NA22") _p = new Na22Generation();

  else if (_name == "Kr83m") _p = new Kr83mGeneration();

  else if (_name == "2PI") _p = new SingleParticle2Pi();

  else if (_name == "MUON_GENERATOR") _p = new MuonGenerator();

  else if (_name == "NEUTRON_GENERATOR") _p = new NeutronGenerator();

  else if (_name == "EL_TABLE_GENERATOR") _p = new ELTableGenerator();

  else if (_name == "SCINT_GENERATOR") _p = new ScintillationGenerator();

  else if (_name == "LABMUON_GENERATOR") _p = new MuonAngleGenerator();

  else if (_name == "NEUTRON_GENERATOR") _p = new NeutronGenerator();

  else if (_name == "E+E-PAIR") _p = new ElecPositronPair();

  else if (_name == "DOUBLE_PARTICLE") _p = new DoubleParticle();

  else if (_name == "BACK2BACK") _p = new Back2backGammas();

  else {
    G4String err = "The user specified an unknown generator: " + _name;
    G4Exception("CreateGenerator()", "[GeneratorFactory]",
        FatalException, err);
  }

  return _p;
}


