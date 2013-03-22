// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 9 Mar 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "PrimaryGeneration.h"

#include <G4VPrimaryGenerator.hh>
#include <G4Event.hh>


using namespace nexus;



PrimaryGeneration::PrimaryGeneration():
  G4VUserPrimaryGeneratorAction(), _generator(0)
{
}



PrimaryGeneration::~PrimaryGeneration()
{
}



void PrimaryGeneration::GeneratePrimaries(G4Event* event)
{
  if (!_generator)
    G4Exception("", "", FatalException, "");

  _generator->GeneratePrimaryVertex(event);
}
