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
#include "BlackBoxSimple1_5.h"
#include "BlackBoxSimple3_5.h"
#include "BlackBoxSimple23_5.h"
#include "BlackBoxSimple25.h"
#include "BlackBoxSimple26_5.h"
#include "BlackBoxSimple28.h"
#include "BlackBoxSimple31.h"
#include "BlackBoxSimple38_5.h"
#include "BlackBoxSimple46.h"
#include "BlackBoxSimple53_5.h"
#include "BlackBoxNew.h"
#include "BlackBoxNew1_5.h"
#include "BlackBoxNew3_5.h"
#include "BlackBoxNew23_5.h"
#include "BlackBoxNew25.h"
#include "BlackBoxNew26_5.h"
#include "BlackBoxNew28.h"
#include "BlackBoxNew31.h"
#include "BlackBoxNew38_5.h"
#include "BlackBoxNew46.h"
#include "BlackBoxNew53_5.h"

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
    
  else if (name_ == "BLACK_BOX_SIMPLE_1_5") p = new BlackBoxSimple1_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_3_5") p = new BlackBoxSimple3_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_23_5") p = new BlackBoxSimple23_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_25") p = new BlackBoxSimple25();
    
  else if (name_ == "BLACK_BOX_SIMPLE_26_5") p = new BlackBoxSimple26_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_28") p = new BlackBoxSimple28();
    
  else if (name_ == "BLACK_BOX_SIMPLE_31") p = new BlackBoxSimple31();
    
  else if (name_ == "BLACK_BOX_SIMPLE_38_5") p = new BlackBoxSimple38_5();
    
  else if (name_ == "BLACK_BOX_SIMPLE_46") p = new BlackBoxSimple46();
    
  else if (name_ == "BLACK_BOX_SIMPLE_53_5") p = new BlackBoxSimple53_5();

  else if (name_ == "BLACK_BOX_NEW") p = new BlackBoxNew();
    
  else if (name_ == "BLACK_BOX_NEW_1_5") p = new BlackBoxNew1_5();
    
  else if (name_ == "BLACK_BOX_NEW_3_5") p = new BlackBoxNew3_5();
    
  else if (name_ == "BLACK_BOX_NEW_23_5") p = new BlackBoxNew23_5();
    
  else if (name_ == "BLACK_BOX_NEW_25") p = new BlackBoxNew25();
    
  else if (name_ == "BLACK_BOX_NEW_26_5") p = new BlackBoxNew26_5();
    
  else if (name_ == "BLACK_BOX_NEW_28") p = new BlackBoxNew28();
    
  else if (name_ == "BLACK_BOX_NEW_31") p = new BlackBoxNew31();
    
  else if (name_ == "BLACK_BOX_NEW_38_5") p = new BlackBoxNew38_5();
    
  else if (name_ == "BLACK_BOX_NEW_46") p = new BlackBoxNew46();
    
  else if (name_ == "BLACK_BOX_NEW_53_5") p = new BlackBoxNew53_5();

  else {
    G4String err = "The user selected an unknown geometry: " + name_;
    G4Exception("[GeometryFactory]", "CreateGeometry", FatalException, err);
  }

  return p;
}
