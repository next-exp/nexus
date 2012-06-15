// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 13 December 2011
//
//  Copyright (c) 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ELLookupTableGenerator.h"

#include "ConfigService.h"
#include "DetectorConstruction.h"
#include "BaseGeometry.h"
#include "IonizationElectron.h"

#include <G4RunManager.hh>
#include <G4PrimaryParticle.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>



namespace nexus {


  ELLookupTableGenerator::ELLookupTableGenerator()
  {
    DetectorConstruction* detconst = (DetectorConstruction*)
      G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    _geom = detconst->GetGeometry();
  }
  
  
  
  ELLookupTableGenerator::~ELLookupTableGenerator()
  {
  }
  
  
  
  void ELLookupTableGenerator::GeneratePrimaryVertex(G4Event* event)
  {
    G4PrimaryParticle* ionielectron =
      new G4PrimaryParticle(IonizationElectron::Definition());

    ionielectron->Set4Momentum(0.,0.,1.,6.999*eV);

    G4ThreeVector position = _geom->GenerateVertex("EL_TABLE");
    const G4double time = 0.;
    G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

    // Add particle to the vertex and this to the event
    vertex->SetPrimary(ionielectron);
    event->AddPrimaryVertex(vertex);
  }
  
  
} // end namespace nexus


