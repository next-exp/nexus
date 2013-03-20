// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 15 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "PersistencyManager.h"

using namespace nexus;



PersistencyManager::PersistencyManager(): 
  G4VPersistencyManager()
{
}



PersistencyManager::~PersistencyManager()
{
}



void PersistencyManager::Initialize()
{
  PersistencyManager* current = dynamic_cast<PersistencyManager*>(G4VPersistencyManager::GetPersistencyManager());

  if (!current) current = new PersistencyManager();
}



G4bool PersistencyManager::Store(const G4Event*)
{
  return true;
}


G4bool PersistencyManager::Store(const G4Run*)
{
  G4cout << "the run" << G4endl;
  return true;
}


G4bool PersistencyManager::Store(const G4VPhysicalVolume*)
{ return true; }