// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 8 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "NexusApp.h"

#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"

#include <G4GenericMessenger.hh>
#include <globals.hh>


using namespace nexus;



NexusApp::NexusApp(G4String)
{
  _msg = new G4GenericMessenger(this, "/nexus/", 
    "NexusApp control commands.");

  _msg->DeclareMethod("SelectGeometry", 
    &NexusApp::CreateDetectorConstruction, "");
  _msg->DeclareMethod("SelectPhysicsList",
    &NexusApp::CreatePhysicsList, "");
  _msg->DeclareMethod("SelectGenerator",
    &NexusApp::CreatePrimaryGeneration, "");
}



NexusApp::~NexusApp()
{
  delete _msg;
}



void NexusApp::Initialize()
{
  G4RunManager::Initialize();
}


////////////////////////////////////////////////////////////

#include "XeSphere.h"


void NexusApp::CreateDetectorConstruction(G4String name)
{
  /// Make sure that no user detector construction class 
  /// has been set already
  if (userDetector) {
    G4Exception("CreateDetectorConstruction()", "[NexusApp]",
      JustWarning, "Detector construction already set.");
    return;
  }

  DetectorConstruction* p = new DetectorConstruction();

  if (name == "XE_SPHERE")
    p->SetGeometry(new XeSphere);

  else {
    G4String errmsg = "User selected an unknown geometry: ";
    errmsg += name;
    G4Exception("CreateDetectorConstruction()", "[NexusApp]",
      FatalException, errmsg);
  }

  this->SetUserInitialization(p);
}


////////////////////////////////////////////////////////////

#include "DefaultPhysicsList.h"


void NexusApp::CreatePhysicsList(G4String name)
{
  if (physicsList) {
    G4Exception("CreatePhysicsList()", "[NexusApp]",
      JustWarning, "Physics list already set.");
    return;
  }

  G4VUserPhysicsList* p = 0;

  if (name == "DEFAULT") 
    p = new DefaultPhysicsList();

  else{
    G4String errmsg = "User selected an unknown physics list: ";
    errmsg += name;
    G4Exception("CreateDetectorConstruction()", "[NexusApp]",
      FatalException, errmsg);
  }

  this->SetUserInitialization(p);
}


////////////////////////////////////////////////////////////

#include "SingleParticle.h"


void NexusApp::CreatePrimaryGeneration(G4String name)
{
  if (userPrimaryGeneratorAction) {
    G4Exception("CreatePrimaryGeneration()", "[NexusApp]",
      JustWarning, "Primary generation already set.");
  }

  PrimaryGeneration* p = new PrimaryGeneration();

  if (name == "SINGLE_PARTICLE")
    p->SetGenerator(new SingleParticle);

  else {
    G4String errmsg = "User selected an unknown generator: ";
    errmsg += name;
    G4Exception("CreatePrimaryGeneration()", "[NexusApp]", 
      FatalException, errmsg);
  }

  this->SetUserAction(p);
}