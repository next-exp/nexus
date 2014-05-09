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
#include "GenbbInterface.h"
#include "SingleParticle2Pi.h"


G4VPrimaryGenerator* GeneratorFactory::CreateGenerator() const
{
  G4VPrimaryGenerator* p = 0;

  if (_name == "SINGLE_PARTICLE") p = new SingleParticle();

  else if (_name == "DECAY0") p = new Decay0Interface();

  else if (_name == "ION_GUN") p = new IonGun();

  else if (_name == "NA22") p = new Na22Generation();

  else if (_name == "GEN_BB") p = new GenbbInterface();

  else if (_name == "2PI") p = new SingleParticle2Pi();

  else {
    G4String err = "The user specified an unknown generator: " + _name;
    G4Exception("CreateGenerator()", "[GeneratorFactory]",
        FatalException, err);
  }

  return p;
}


