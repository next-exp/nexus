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
#include "FullRingInfinity.h"
#include "FullRingTiles.h"
#include "Lab_vertices.h"
#include "Lab.h"

BaseGeometry* GeometryFactory::CreateGeometry() const
{
  BaseGeometry* p = 0;

  if (_name == "FULLRING") p = new FullRingInfinity();

  else if (_name == "RING_TILES") p = new FullRingTiles();

  else if (_name == "VERTICES") p = new Lab_vertices();

  else if (_name == "PETALO") p = new Lab();

  else {
    G4String err = "The user selected an unknown geometry: " + _name;
    G4Exception("CreateGeometry", "[BaseGeometry]", FatalException, err);
  }

  return p;
}
