// ---------------------------------------------------------------------------
//
//  Author : Miquel Nebot Guinot <miquel.nebot@ific.uv.es>        
//  Created: Aug 2016
//
//  Copyright (c) 2016 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------

#include "NeutronGenerator.h"

#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryVertex.hh>


using namespace nexus;


NeutronGenerator::NeutronGenerator():
  G4VPrimaryGenerator(), _msg(0), _particle_definition(0),
  _geom(0)
{
  _msg = new G4GenericMessenger(this, "/Generator/NeutronGenerator/",
				"Control commands of the neutron primary generator.");

  _msg->DeclareProperty("region", _region, 
    "Set the region of the geometry where the vertex will be generated.");
 
  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detconst->GetGeometry();
}



NeutronGenerator::~NeutronGenerator()
{
  delete _msg;
}

void NeutronGenerator::GeneratePrimaryVertex(G4Event* event)
{
  // _particle_definition = G4IonTable::GetIonTable()->GetIon(_z, _a, 0.);
  
  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[NeutronGenerator]",
		FatalException, " can not create neutron "); 

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
