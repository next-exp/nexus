// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 13 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "DefaultRunAction.h"

#include <G4Run.hh>

using namespace nexus;



DefaultRunAction::DefaultRunAction(): G4UserRunAction()
{
}



DefaultRunAction::~DefaultRunAction()
{
}



void DefaultRunAction::BeginOfRunAction(const G4Run* run)
{
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
}


void DefaultRunAction::EndOfRunAction(const G4Run*)
{
  G4cout << "### Run " << run->GetRunID() << " end." << G4endl;
}
