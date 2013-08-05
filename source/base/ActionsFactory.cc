// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 13 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "ActionsFactory.h"

#include <G4GenericMessenger.hh>
#include <G4UserRunAction.hh>
#include <G4UserEventAction.hh>
#include <G4UserTrackingAction.hh>
#include <G4UserSteppingAction.hh>
#include <G4UserStackingAction.hh>

using namespace nexus;


ActionsFactory::ActionsFactory(): _msg(0)
{
  _msg = new G4GenericMessenger(this, "/Actions/");
  _msg->DeclareProperty("RegisterRunAction",      _runact_name, "");
  _msg->DeclareProperty("RegisterEventAction",    _evtact_name, "");
  _msg->DeclareProperty("RegisterTrackingAction", _trkact_name, "");
  _msg->DeclareProperty("RegisterSteppingAction", _stpact_name, "");
  _msg->DeclareProperty("RegisterStackingAction", _stkact_name, "");
}


ActionsFactory::~ActionsFactory()
{
  delete _msg;
}


//////////////////////////////////////////////////////////////////////

#include "DefaultRunAction.h"


G4UserRunAction* ActionsFactory::CreateRunAction() const
{
  G4UserRunAction* p = 0;

  if (_runact_name == "DEFAULT") p = new DefaultRunAction();

  return p;
}


//////////////////////////////////////////////////////////////////////

//#include "DefaultEventAction.h"


G4UserEventAction* ActionsFactory::CreateEventAction() const
{
  G4UserEventAction* p = 0;
  return p;
}


//////////////////////////////////////////////////////////////////////

#include "DefaultTrackingAction.h"


G4UserTrackingAction* ActionsFactory::CreateTrackingAction() const
{
  G4UserTrackingAction* p = 0;

  if (_trkact_name == "DEFAULT") p = new DefaultTrackingAction();

  else {
    G4String err = "Unknown user tracking action: " + _trkact_name;
    G4Exception("CreateTrackingAction()","[ActionsFactory]",JustWarning,err);
  }
  
  return p;
}


//////////////////////////////////////////////////////////////////////

//#include "DefaultSteppingAction.h"


G4UserSteppingAction* ActionsFactory::CreateSteppingAction() const
{
  G4UserSteppingAction* p = 0;
  return p;
}


//////////////////////////////////////////////////////////////////////

//#include "DefaultStackingAction.h"


G4UserStackingAction* ActionsFactory::CreateStackingAction() const
{
  G4UserStackingAction* p = 0;
  return p;
}

