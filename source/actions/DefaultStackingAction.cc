// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 25 August 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "DefaultStackingAction.h"


using namespace nexus;



DefaultStackingAction::DefaultStackingAction(): G4UserStackingAction()
{
}



DefaultStackingAction::~DefaultStackingAction()
{
}



G4ClassificationOfNewTrack 
DefaultStackingAction::ClassifyNewTrack(const G4Track* /*track*/)
{
  return fUrgent;
}



void DefaultStackingAction::NewStage()
{
  return;
}



void DefaultStackingAction::PrepareNewEvent()
{
  return;
}
