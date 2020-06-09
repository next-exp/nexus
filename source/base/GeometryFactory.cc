// ----------------------------------------------------------------------------
// nexus | GeometryFactory.cc
//
// This class instantiates the main geometry of the simulation that the user
// specifies via configuration parameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "GeometryFactory.h"

#include "BaseGeometry.h"
#include <G4GenericMessenger.hh>


using namespace nexus;


GeometryFactory::GeometryFactory(): msg_(0)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/");
  msg_->DeclareProperty("RegisterGeometry", name_, "");
}


GeometryFactory::~GeometryFactory()
{
  delete msg_;
}


//////////////////////////////////////////////////////////////////////
#include "FullRingInfinity.h"
#include "FullRingTiles.h"
#include "Lab_vertices.h"
#include "Lab.h"
#include "PetBox.h"

BaseGeometry* GeometryFactory::CreateGeometry() const
{
  BaseGeometry* p = 0;

  if (name_ == "FULLRING") p = new FullRingInfinity();

  else if (name_ == "RING_TILES") p = new FullRingTiles();

  else if (name_ == "VERTICES") p = new Lab_vertices();

  else if (name_ == "PETALO") p = new Lab();

  else if (_name == "PETBOX") p = new PetBox();

  else {
    G4String err = "The user selected an unknown geometry: " + name_;
    G4Exception("[GeometryFactory]", "CreateGeometry()", FatalException, err);
  }

  return p;
}
