// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 8 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "NexusApp.h"

#include "GeometryFactory.h"
#include "GeneratorFactory.h"
#include "ActionsFactory.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"
#include "PersistencyManager.h"
#include "BatchSession.h"

#include <G4GenericPhysicsList.hh>
#include <G4UImanager.hh>

using namespace nexus;



NexusApp::NexusApp(G4String init_macro): G4RunManager()
{
  // Create a configure the generic messenger for the app
  _msg = new G4GenericMessenger(this, "/nexus/", "Nexus control commands.");
  _msg->DeclareMethod("RegisterMacro", &NexusApp::RegisterMacro, ""); 


  // Create the necessary classes for initialization

  physicsList = new G4GenericPhysicsList();

  _geom_fctr  = new GeometryFactory();
  _gen_fctr   = new GeneratorFactory();
  _act_fctr   = new ActionsFactory();
 
  BatchSession* batch = new BatchSession(init_macro.c_str());
  
  batch->SessionStart();

  //G4UImanager::GetUIpointer()->ExecuteMacroFile(init_macro);

  // Initialization classes set in the G4RunManager

  this->SetUserInitialization(physicsList);

  DetectorConstruction* dc = new DetectorConstruction();
  dc->SetGeometry(_geom_fctr->CreateGeometry());
  this->SetUserInitialization(dc);

  PrimaryGeneration* pg = new PrimaryGeneration();
  pg->SetGenerator(_gen_fctr->CreateGenerator());
  this->SetUserAction(pg);

  G4UImanager* UI = G4UImanager::GetUIpointer();

  if (UI->GetCurrentValues("/Actions/RegisterTrackingAction"))
    this->SetUserAction(_act_fctr->CreateTrackingAction());


  PersistencyManager::Initialize();
}



NexusApp::~NexusApp()
{
  PersistencyManager* current = dynamic_cast<PersistencyManager*>(G4VPersistencyManager::GetPersistencyManager());
  current->CloseFile();
  delete _msg;
}



void NexusApp::RegisterMacro(const G4String& macro)
{
  _macros.push_back(macro);
}



void NexusApp::Initialize()
{
  // Execute command macro file before initializing the app
  // so that all classes get configured
  G4UImanager* UI = G4UImanager::GetUIpointer();
  for (unsigned int i=0; i<_macros.size(); i++)
    ExecuteMacroFile(_macros[i].data());

  // Initialize the run manager
  G4RunManager::Initialize();
}



void NexusApp::ExecuteMacroFile(const char* filename)
{
  G4UImanager* UI = G4UImanager::GetUIpointer();
  G4UIsession* batchSession = new BatchSession(filename, UI->GetSession());
  UI->SetSession(batchSession);
  G4UIsession* previousSession = UI->GetSession()->SessionStart();
  delete UI->GetSession();
  UI->SetSession(previousSession);
}