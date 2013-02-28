// -----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 15 Apr 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// -----------------------------------------------------------------------------

#include "NexusFactory.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"

#include <G4GenericMessenger.hh>

using namespace nexus;



NexusFactory::NexusFactory()
{
  _messenger = new G4GenericMessenger(this, "/NexusFactory/", 
    "NexusFactory control commands.");

  G4GenericMessenger::Command& cmd = 
    _messenger->DeclareProperty("Geometry", _geometry_name,
      "Specify a geometry for the factory to build.");

  cmd = _messenger->DeclareProperty("PhysicsList", _physics_list_name,
    "Specify a physics list for the factory to build.");

  cmd = _messenger->DeclareProperty("Generator", _generator_name,
    "Specify a generator for the factory to build.");

  cmd = _messenger->DeclareProperty("RunAction", _run_action_name,
    "Specify a run action for the factory to build.");

  cmd = _messenger->DeclareProperty("EventAction", _event_action_name,
    "Specify an event action for the factory to build.");

  cmd = _messenger->DeclareProperty("TrackingAction", _tracking_action_name,
    "Specify a tracking action for the factory to build.");

  cmd = _messenger->DeclareProperty("SteppingAction", _stepping_action_name,
    "Specify a stepping action for the factory to build.");
}


NexusFactory::~NexusFactory()
{
  delete _messenger;
}



// GEOMETRIES ////////////////////////////////////////////////////////

#include "CylindricChamber.h"
#include "SquareChamber.h"
#include "XeSphere.h"
#include "Next100.h"
#include "Next100OpticalGeometry.h"
#include "Next0Ific.h"
#include "Next1EL.h"
#include "Next1Lbnl.h"
#include "GraXe.h"
#include "PMT_QE_setup.h"
  
DetectorConstruction* NexusFactory::CreateDetectorConstruction()
{
  DetectorConstruction* p = new DetectorConstruction();

  if      (_geometry_name == "NEXT100")
    p->SetGeometry(new Next100);

  else if (_geometry_name == "NEXT100_OPT")
    p->SetGeometry(new Next100OpticalGeometry);

  else if (_geometry_name == "NEXT1EL")
    p->SetGeometry(new Next1EL);

  else if (_geometry_name == "NEXT_LBNL")
    p->SetGeometry(new Next1Lbnl);
  
  else if (_geometry_name == "GRAXE")
    p->SetGeometry(new GraXe);
  
  else if (_geometry_name == "CYLINDRIC_CHAMBER")  
    p->SetGeometry(new CylindricChamber);
  
  else if (_geometry_name == "SQUARE_CHAMBER")
    p->SetGeometry(new SquareChamber);
  
  else if (_geometry_name == "XE_SPHERE")
    p->SetGeometry(new XeSphere);
  
  else if (_geometry_name == "NEXT0_IFIC")
    p->SetGeometry(new Next0Ific);

  else if (_geometry_name == "PMT_QE_SETUP")
    p->SetGeometry(new PMT_QE_setup);
  
  else
    G4Exception("[NexusFactory]", "CreateDetectorConstruction()", 
		FatalException, 
		" ERROR: Unknown geometry selected in config file.");
  
  return p;
}


// PHYSICS LISTS /////////////////////////////////////////////////////

#include "DefaultPhysicsList.h"

G4VUserPhysicsList* NexusFactory::CreatePhysicsList()
{
  G4VUserPhysicsList* p = 0;
  
  if (_physics_list_name == "DEFAULT")
    p = new DefaultPhysicsList(); 

  else
    G4Exception("[NexusFactory]", "CreatePhysicsList()", FatalException,
		" ERROR: Unkown physics list selected in config file.");
  
  return p;
}


// PRIMARY GENERATORS ///////////////////////////////////////////////

#include "SingleParticle.h"
#include "GenbbInterface.h"
#include "Na22Generation.h"
#include "ELLookupTableGenerator.h"

PrimaryGeneration* NexusFactory::CreatePrimaryGeneration()
{
  PrimaryGeneration* p = new PrimaryGeneration();
    
  if      (_generator_name == "SINGLE_PARTICLE")
    p->SetGenerator(new SingleParticle); 
  
  else if (_generator_name == "GENBB_INTERFACE")
    p->SetGenerator(new GenbbInterface);
  
  else if (_generator_name == "NA22_GENERATOR")
    p->SetGenerator(new Na22Generation);

  else if (_generator_name == "EL_LOOKUP_TABLE")
    p->SetGenerator(new ELLookupTableGenerator);

  else
    G4Exception("[NexusFactory]", "CreatePrimaryGeneration()", FatalException,
		" ERROR: Unknown primary generator selected in config file.");
  
  return p;
}


// RUN ACTIONS //////////////////////////////////////////////////////

#include "DefaultRunAction.h"

G4UserRunAction* NexusFactory::CreateRunAction()
{
  G4UserRunAction* p = 0;
    
  if      (_run_action_name == "DEFAULT")
    p = new DefaultRunAction; 
  
  else
    G4Exception("[NexusFactory]", "CreateRunAction()", FatalException,
		" ERROR: Unknown run action selected in config file.");

  return p;
}



// EVENT ACTIONS /////////////////////////////////////////////////////

#include "DefaultEventAction.h"
#include "FastSimFiltersEventAction.h"

G4UserEventAction* NexusFactory::CreateEventAction()
{
  G4UserEventAction* p = 0;
    
  if      (_event_action_name == "DEFAULT") 
    p = new DefaultEventAction; 

  else if (_event_action_name == "FAST_SIM_FILTERS")
    p = new FastSimFiltersEventAction;
  
  else 
    G4Exception("[NexusFactory]", "CreateEventAction()", FatalException,
		" ERROR: Unknown event action selected in config file.");
    
  return p;
}


// TRACKING ACTIONS //////////////////////////////////////////////////

#include "DefaultTrackingAction.h"
#include "ELTablesTrackingAction.h"

G4UserTrackingAction* NexusFactory::CreateTrackingAction()
{
  G4UserTrackingAction* p = 0;
    
  if      (_tracking_action_name == "DEFAULT")
    p = new DefaultTrackingAction; 

  else if (_tracking_action_name == "EL_TABLES")
    p = new ELTablesTrackingAction;

  else 
    G4Exception("[NexusFactory]", "CreateTrackingAction()", FatalException,
		" ERROR: Unknown tracking action selected in config file.");
  
  return p;
}


// STEPPING ACTIONS //////////////////////////////////////////////////

#include "DefaultSteppingAction.h"

G4UserSteppingAction* NexusFactory::CreateSteppingAction()
{
  G4UserSteppingAction* p = 0;
    
  if      (_stepping_action_name == "DEFAULT") 
    p = new DefaultSteppingAction;

  else
    G4Exception("[NexusFactory]", "CreateSteppingAction()", FatalException,
		" ERROR: Unknown stepping action selected in config file.");
    
  return p;
}

