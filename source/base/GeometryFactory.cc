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

#include "XeSphere.h"
#include "CylindricChamber.h"
#include "Next1EL.h"
#include "Next100.h"
#include "Next100OpticalGeometry.h"
#include "NextNew.h"
#include "MagBox.h"
#include "NextTonScale.h"
#include "NextFlex.h"

BaseGeometry* GeometryFactory::CreateGeometry() const
{
  BaseGeometry* p = 0;

  if      (name_ == "XE_SPHERE") p = new XeSphere();

  else if (name_ == "CYLINDRIC_CHAMBER") p = new CylindricChamber();

  else if (name_ == "NEXT1_EL") p = new Next1EL();

  else if (name_ == "NEXT100") p = new Next100();

  else if (name_ == "NEXT100_OPT") p = new Next100OpticalGeometry();

  else if (name_ == "NEXT_NEW") p = new NextNew();

  else if (name_ == "MAG_BOX") p = new MagBox();

  else if (name_ == "TON_SCALE") p = new NextTonScale();

  else if (_name == "NEXT_FLEX") p = new NextFlex();

  else {
    G4String err = "The user selected an unknown geometry: " + name_;
    G4Exception("[GeometryFactory]", "CreateGeometry", FatalException, err);
  }

  return p;
}
