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
#include "GenbbInterface.h"
#include "IonGun.h"


G4VPrimaryGenerator* GeneratorFactory::CreateGenerator() const
{
  G4VPrimaryGenerator* p = 0;

  if (_name == "SINGLE_PARTICLE") p = new SingleParticle();

  else if (_name == "GENBB_INTERFACE") p = new GenbbInterface();

  else if (_name == "ION_GUN") p = new IonGun();

  return p;
}


