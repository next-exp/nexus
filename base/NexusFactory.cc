// -----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 15 Apr 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NexusFactory.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"

using namespace nexus;



// PHYSICS LISTS -----------------------------------------------------

#include "EMStandardPhysicsList.h"
#include "LowEnergyEMPhysicsList.h"
#include "OpticalPhysicsList.h"

G4VUserPhysicsList* 
NexusFactory::CreatePhysicsList(const G4String& name)
{
  G4VUserPhysicsList* p = 0;

  if      (name == "EM_STANDARD")
    p = new EMStandardPhysicsList(); 

  else if (name == "LOW_ENERGY_EM")
    p = new LowEnergyEMPhysicsList();
  
  else if (name == "OPTICAL")
    p = new OpticalPhysicsList(); 

  else
    G4Exception("[NexusFactory] ERROR: Unknown physics list!");
  
  return p;
}



// PRIMARY GENERATORS ------------------------------------------------

#include "SingleParticle.h"
#include "GenbbInterface.h"

PrimaryGeneration* 
NexusFactory::CreatePrimaryGeneration(const G4String& name)
{
  PrimaryGeneration* p = new PrimaryGeneration();
    
  if      (name == "SINGLE_PARTICLE")
    p->SetGenerator(new SingleParticle); 
  
  else if (name == "GENBB_INTERFACE")
    p->SetGenerator(new GenbbInterface);
  
  else
    G4Exception("[NexusFactory] ERROR: Unknown generator!");

  return p;
}



// GEOMETRIES --------------------------------------------------------

#include "CylindricChamber.h"
#include "SquareChamber.h"
#include "XeSphere.h"
#include "Next0Ific.h"
#include "Next1EL.h"
#include "PmtR7378A.h"
  
DetectorConstruction* 
NexusFactory::CreateDetectorConstruction(const G4String& name)
{
  DetectorConstruction* p = new DetectorConstruction();
  
  if      (name == "CYLINDRIC_CHAMBER")  
    p->SetGeometry(new CylindricChamber);

  else if (name == "SQUARE_CHAMBER")
    p->SetGeometry(new SquareChamber);
  
  else if (name == "XE_SPHERE")
    p->SetGeometry(new XeSphere);
  
  else if (name == "NEXT0_IFIC")
    p->SetGeometry(new Next0Ific);

  else if (name == "NEXT1_EL")
    p->SetGeometry(new Next1EL);
  
  else 
    G4Exception("[NexusFactory] ERROR: Unknown geometry!");
  
  return p;
}



// RUN ACTIONS -------------------------------------------------------

#include "DefaultRunAction.h"

G4UserRunAction* 
NexusFactory::CreateRunAction(const G4String& name)
{
  G4UserRunAction* p = 0;
    
  if      (name == "DEFAULT")
    p = new DefaultRunAction; 
  
  else
    G4Exception("[NexusFactory] ERROR: Unknown run action!");

  return p;
}



// EVENT ACTIONS -----------------------------------------------------

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
    G4Exception("[NexusFactory] ERROR: Unknown event action!");
    
  return p;
}
  
  

// TRACKING ACTIONS --------------------------------------------------

#include "DefaultTrackingAction.h"

G4UserTrackingAction* 
NexusFactory::CreateTrackingAction(const G4String& name)
{
  G4UserTrackingAction* p = 0;
    
  if      (name == "DEFAULT")
    p = new DefaultTrackingAction; 
  
  else 
    G4Exception("[NexusFactory] ERROR: Unknown tracking action!");
    
  return p;
}



// STEPPING ACTIONS --------------------------------------------------

#include "DefaultSteppingAction.h"

G4UserSteppingAction* 
NexusFactory::CreateSteppingAction(const G4String& name)
{
  G4UserSteppingAction* p = 0;
    
  if      (name == "DEFAULT") 
    p = new DefaultSteppingAction;

  else
    G4Exception("[NexusFactory] ERROR: Unknown stepping action!");
    
  return p;
}
