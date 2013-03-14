// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 9 Mar 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "PrimaryGeneration.h"

#include <G4GenericMessenger.hh>
#include <G4VPrimaryGenerator.hh>
#include <G4Event.hh>


using namespace nexus;


PrimaryGeneration::PrimaryGeneration():
  G4VUserPrimaryGeneratorAction(), _msg(0), _generator(0)
{
  // _msg = new G4GenericMessenger(this, "/Generator/");
  // _msg->DeclareMethod("RegisterGenerator",
  //   &PrimaryGeneration::CreateGenerator, "");
}



PrimaryGeneration::~PrimaryGeneration()
{
//  delete _msg;
}



void PrimaryGeneration::GeneratePrimaries(G4Event* event)
{
  if (_generator == 0)
    G4Exception("", "", FatalException, "");
      //G4Exception("[DetectorConstruction] ERROR: Primary generator not set!");

  _generator->GeneratePrimaryVertex(event);
}


//////////////////////////////////////////////////////////////////////


// #include "SingleParticle.h"


// void PrimaryGeneration::CreateGenerator(G4String name)
// {
//    if (_generator)
//     G4Exception("[PrimaryGeneration]", "CreateGenerator()",
//       JustWarning, "Generator was already set!");

//   // Geometry factory

//   if (name == "SINGLE_PARTICLE")
//     _generator = new SingleParticle();

//   else
//     G4Exception("[PrimaryGeneration]", "CreateGenerator()",
//       FatalException, "Unknown generator!");

// }
