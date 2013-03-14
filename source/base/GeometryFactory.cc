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


BaseGeometry* GeometryFactory::CreateGeometry() const
{
  BaseGeometry* p = 0;

  if (_name == "XE_SPHERE") p = new XeSphere();

  return p;
}


