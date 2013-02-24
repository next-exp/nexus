// -----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 15 Apr 2009
//
//  Copyright (c) 2009-2012 NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NexusFactory.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"
#include "NexusFactoryMessenger.h"

using namespace nexus;


NexusFactory::NexusFactory()
{
  _messenger = new NexusFactoryMessenger(this);
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
  
DetectorConstruction* 
NexusFactory::CreateDetectorConstruction(const G4String& name)
{
  DetectorConstruction* p = new DetectorConstruction();

  if      (name == "NEXT100")
    p->SetGeometry(new Next100);

  else if (name == "NEXT100_OPT")
    p->SetGeometry(new Next100OpticalGeometry);

  else if (name == "NEXT1EL")
    p->SetGeometry(new Next1EL);

  else if (name == "NEXT_LBNL")
    p->SetGeometry(new Next1Lbnl);
  
  else if (name == "GRAXE")
    p->SetGeometry(new GraXe);
  
  else if (name == "CYLINDRIC_CHAMBER")  
    p->SetGeometry(new CylindricChamber);
  
  else if (name == "SQUARE_CHAMBER")
    p->SetGeometry(new SquareChamber);
  
  else if (name == "XE_SPHERE")
    p->SetGeometry(new XeSphere);
  
  else if (name == "NEXT0_IFIC")
    p->SetGeometry(new Next0Ific);

  else if (name == "PMT_QE_SETUP")
    p->SetGeometry(new PMT_QE_setup);
  
  else
    G4Exception("[NexusFactory]", "CreateDetectorConstruction()", 
		FatalException, 
		" ERROR: Unknown geometry selected in config file.");
  
  return p;
}


// PHYSICS LISTS /////////////////////////////////////////////////////

#include "DefaultPhysicsList.h"

G4VUserPhysicsList* 
NexusFactory::CreatePhysicsList(const G4String& name)
{
  G4VUserPhysicsList* p = 0;
  
  if (name == "DEFAULT")
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

PrimaryGeneration* 
NexusFactory::CreatePrimaryGeneration(const G4String& name)
{
  PrimaryGeneration* p = new PrimaryGeneration();
    
  if      (name == "SINGLE_PARTICLE")
    p->SetGenerator(new SingleParticle); 
  
  else if (name == "GENBB_INTERFACE")
    p->SetGenerator(new GenbbInterface);
  
  else if (name == "NA22_GENERATOR")
    p->SetGenerator(new Na22Generation);

  else if (name == "EL_LOOKUP_TABLE")
    p->SetGenerator(new ELLookupTableGenerator);

  else
    G4Exception("[NexusFactory]", "CreatePrimaryGeneration()", FatalException,
		" ERROR: Unknown primary generator selected in config file.");
  
  return p;
}


// RUN ACTIONS //////////////////////////////////////////////////////

#include "DefaultRunAction.h"

G4UserRunAction* 
NexusFactory::CreateRunAction(const G4String& name)
{
  G4UserRunAction* p = 0;
    
  if      (name == "DEFAULT")
    p = new DefaultRunAction; 
  
  else
    G4Exception("[NexusFactory]", "CreateRunAction()", FatalException,
		" ERROR: Unknown run action selected in config file.");

  return p;
}



// EVENT ACTIONS /////////////////////////////////////////////////////

#include "DefaultEventAction.h"
#include "FastSimFiltersEventAction.h"

G4UserEventAction* 
NexusFactory::CreateEventAction(const G4String& name)
{
  G4UserEventAction* p = 0;
    
  if      (name == "DEFAULT") 
    p = new DefaultEventAction; 

  else if (name == "FAST_SIM_FILTERS")
    p = new FastSimFiltersEventAction;
  
  else 
    G4Exception("[NexusFactory]", "CreateEventAction()", FatalException,
		" ERROR: Unknown event action selected in config file.");
    
  return p;
}


// TRACKING ACTIONS //////////////////////////////////////////////////

#include "DefaultTrackingAction.h"
#include "ELTablesTrackingAction.h"

G4UserTrackingAction* 
NexusFactory::CreateTrackingAction(const G4String& name)
{
  G4UserTrackingAction* p = 0;
    
  if      (name == "DEFAULT")
    p = new DefaultTrackingAction; 

  else if (name == "EL_TABLES")
    p = new ELTablesTrackingAction;

  else 
    G4Exception("[NexusFactory]", "CreateTrackingAction()", FatalException,
		" ERROR: Unknown tracking action selected in config file.");
  
  return p;
}


// STEPPING ACTIONS //////////////////////////////////////////////////

#include "DefaultSteppingAction.h"

G4UserSteppingAction* 
NexusFactory::CreateSteppingAction(const G4String& name)
{
  G4UserSteppingAction* p = 0;
    
  if      (name == "DEFAULT") 
    p = new DefaultSteppingAction;

  else
    G4Exception("[NexusFactory]", "CreateSteppingAction()", FatalException,
		" ERROR: Unknown stepping action selected in config file.");
    
  return p;
}

