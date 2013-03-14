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
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"

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
  _geomfctr   = new GeometryFactory();
  _genfctr    = new GeneratorFactory();

 
  G4UImanager::GetUIpointer()->
    ApplyCommand("/control/execute " + init_macro);

  // Initialization classes set in the G4RunManager

  this->SetUserInitialization(physicsList);

  DetectorConstruction* dc = new DetectorConstruction();
  dc->SetGeometry(_geomfctr->CreateGeometry());
  this->SetUserInitialization(dc);

  PrimaryGeneration* pg = new PrimaryGeneration();
  pg->SetGenerator(_genfctr->CreateGenerator());
  this->SetUserAction(pg);
}



NexusApp::~NexusApp()
{
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
    UI->ExecuteMacroFile(_macros[i]);

  // Initialize the run manager
  G4RunManager::Initialize();
}
