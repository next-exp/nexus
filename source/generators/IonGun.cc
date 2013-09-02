// ---------------------------------------------------------------------------
//  $Id$
//
//  Author : Miquel Nebot Guinot <miquel.nebot@ific.uv.es>        
//  Created: 01 Aug 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------

#include "IonGun.h"

#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>


using namespace nexus;


IonGun::IonGun():
  G4VPrimaryGenerator(), _msg(0), _particle_definition(0),
  _z(0.), _a(0.), _geom(0)
{
  _msg = new G4GenericMessenger(this, "/Generator/IonGun/",
				"Control commands of the ion gun primary generator.");

  G4GenericMessenger::Command& z_ =
    _msg->DeclareProperty("atomic_number", _z, "Set atomic number");
  z_.SetParameterName("atomic_number", false);
 
  G4GenericMessenger::Command& a_ =
    _msg->DeclareProperty("mass_number", _a, "Set mass number");
   a_.SetParameterName("mass_number", false);

  _msg->DeclareProperty("region", _region, 
    "Set the region of the geometry where the vertex will be generated.");
 
  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detconst->GetGeometry();
}



IonGun::~IonGun()
{
  delete _msg;
}
// void IonGun::SetParticleDefinition()
// {
 
// }

void IonGun::GeneratePrimaryVertex(G4Event* event)
{
 _particle_definition = G4ParticleTable::GetParticleTable()->
    GetIon(_z, _a, 0.);
  
  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[IonGun]",
		FatalException, " can not create ion "); 

  // Generate an initial position for the particle using the geometry
  G4ThreeVector position = _geom->GenerateVertex(_region);
  // Particle generated at start-of-event
  G4double time = 0.;
  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
  
  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle = 
    new G4PrimaryParticle(_particle_definition);
 
  // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);
  event->AddPrimaryVertex(vertex);
}
