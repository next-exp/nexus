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
#include "BlackBoxSimple.h"
#include "BlackBoxSingle.h"
#include "BlackBoxSIMPLE1_5.h"
#include "BlackBoxSIMPLE3_5.h"
#include "BlackBoxSIMPLE23_5.h"
#include "BlackBoxSIMPLE25.h"
#include "BlackBoxSIMPLE26_5.h"
#include "BlackBoxSIMPLE28.h"
#include "BlackBoxSIMPLE31.h"
#include "BlackBoxSIMPLE38_5.h"
#include "BlackBoxSIMPLE46.h"
#include "BlackBoxSIMPLE53_5.h"
#include "BlackBoxNew.h"
#include "BlackBoxNEW1_5.h"
#include "BlackBoxNEW3_5.h"
#include "BlackBoxNEW23_5.h"
#include "BlackBoxNEW25.h"
#include "BlackBoxNEW26_5.h"
#include "BlackBoxNEW28.h"
#include "BlackBoxNEW31.h"
#include "BlackBoxNEW38_5.h"
#include "BlackBoxNEW46.h"
#include "BlackBoxNEW53_5.h"

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

  else if (name_ == "NEXT_FLEX") p = new NextFlex();

  else if (name_ == "BLACK_BOX_SIMPLE") p = new BlackBoxSimple();

  else if (name_ == "BLACK_BOX_SINGLE") p = new BlackBoxSingle();
    
  else if (name_ == "BLACK_BOX_SIMPLE_1_5") p = new BlackBoxSIMPLE1_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_3_5") p = new BlackBoxSIMPLE3_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_23_5") p = new BlackBoxSIMPLE23_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_25") p = new BlackBoxSIMPLE25();
    
  else if (name_ == "BLACK_BOX_SIMPLE_26_5") p = new BlackBoxSIMPLE26_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_28") p = new BlackBoxSIMPLE28();
    
  else if (name_ == "BLACK_BOX_SIMPLE_31") p = new BlackBoxSIMPLE31();
    
  else if (name_ == "BLACK_BOX_SIMPLE_38_5") p = new BlackBoxSIMPLE38_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_46") p = new BlackBoxSIMPLE46();
    
  else if (name_ == "BLACK_BOX_SIMPLE_53_5") p = new BlackBoxSIMPLE53_5();

  else if (name_ == "BLACK_BOX_NEW") p = new BlackBoxNew();
    
  else if (name_ == "BLACK_BOX_NEW_1_5") p = new BlackBoxNEW1_5();
    
  else if (name_ == "BLACK_BOX_NEW_3_5") p = new BlackBoxNEW3_5();
    
  else if (name_ == "BLACK_BOX_NEW_23_5") p = new BlackBoxNEW23_5();
    
  else if (name_ == "BLACK_BOX_NEW_25") p = new BlackBoxNEW25();
    
  else if (name_ == "BLACK_BOX_NEW_26_5") p = new BlackBoxNEW26_5();
    
  else if (name_ == "BLACK_BOX_NEW_28") p = new BlackBoxNEW28();
    
  else if (name_ == "BLACK_BOX_NEW_31") p = new BlackBoxNEW31();
    
  else if (name_ == "BLACK_BOX_NEW_38_5") p = new BlackBoxNEW38_5();
    
  else if (name_ == "BLACK_BOX_NEW_46") p = new BlackBoxNEW46();
    
  else if (name_ == "BLACK_BOX_NEW_53_5") p = new BlackBoxNEW53_5();

  else {
    G4String err = "The user selected an unknown geometry: " + name_;
    G4Exception("[GeometryFactory]", "CreateGeometry", FatalException, err);
  }

  return p;
}
