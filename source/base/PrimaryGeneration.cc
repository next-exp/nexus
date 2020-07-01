// ----------------------------------------------------------------------------
// nexus | PrimaryGeneration.cc
//
// This is a mandatory class which initializes the generation of
// primary particles in a nexus event.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PrimaryGeneration.h"

#include <G4VPrimaryGenerator.hh>
#include <G4Event.hh>


using namespace nexus;



PrimaryGeneration::PrimaryGeneration():
  G4VUserPrimaryGeneratorAction(), generator_(0)
{
}



PrimaryGeneration::~PrimaryGeneration()
{
}



void PrimaryGeneration::GeneratePrimaries(G4Event* event)
{
  if (!generator_)
    G4Exception("", "", FatalException, "");

  generator_->GeneratePrimaryVertex(event);
}
