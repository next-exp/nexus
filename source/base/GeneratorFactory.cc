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
#include "IonGun.h"
#include "Na22Generation.h"
#include "Kr83mGeneration.h"
#include "ScintillationGenerator.h"
#include "DoubleParticle.h"
#include "Back2backGammas.h"
#include "SensMap.h"

G4VPrimaryGenerator* GeneratorFactory::CreateGenerator()
{
  //G4VPrimaryGenerator* p = 0;

  if (_name == "SINGLE_PARTICLE") _p = new SingleParticle();

  else if (_name == "ION_GUN") _p = new IonGun();

  else if (_name == "NA22") _p = new Na22Generation();

  else if (_name == "Kr83m") _p = new Kr83mGeneration();

  else if (_name == "SCINT_GENERATOR") _p = new ScintillationGenerator();

  else if (_name == "DOUBLE_PARTICLE") _p = new DoubleParticle();

  else if (_name == "BACK2BACK") _p = new Back2backGammas();

  else if (_name == "SENSMAP") _p = new SensMap();

  else {
    G4String err = "The user specified an unknown generator: " + _name;
    G4Exception("CreateGenerator()", "[GeneratorFactory]",
        FatalException, err);
  }

  return _p;
}
