// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 13 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "GeometryFactory.h"

#include "BaseGeometry.h"
#include <G4GenericMessenger.hh>


using namespace nexus;


GeometryFactory::GeometryFactory(): _msg(0)
{
  _msg = new G4GenericMessenger(this, "/Geometry/");
  _msg->DeclareProperty("RegisterGeometry", _name, "");
}


GeometryFactory::~GeometryFactory()
{
  delete _msg;
}


//////////////////////////////////////////////////////////////////////

#include "XeSphere.h"
#include "Next100.h"
#include "NEW.h"
#include "Next1EL.h"


BaseGeometry* GeometryFactory::CreateGeometry() const
{
  BaseGeometry* p = 0;

  if (_name == "XE_SPHERE") p = new XeSphere();
  
  else if (_name == "NEXT1_EL") p = new Next1EL();

  else if (_name == "NEXT100") p = new Next100();

  else if (_name == "NEW") p = new NEW();

  return p;
}


