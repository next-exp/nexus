// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 24 August 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "DefaultSteppingAction.h"

#include <G4Step.hh>
#include <G4SteppingManager.hh>

using namespace nexus;



DefaultSteppingAction::DefaultSteppingAction(): G4UserSteppingAction()
{
}



DefaultSteppingAction::~DefaultSteppingAction()
{
}



void DefaultSteppingAction::UserSteppingAction(const G4Step* /*step*/)
{
  return;
}
