// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 27 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "TrajectoryPoint.h"

using namespace nexus;


G4Allocator<TrajectoryPoint> TrjPointAllocator;


TrajectoryPoint::TrajectoryPoint(): 
  _position(0.,0.,0.), _time(0.)
{
}



TrajectoryPoint::TrajectoryPoint(G4ThreeVector pos, G4double t):
  _position(pos), _time(t)
{
}



TrajectoryPoint::TrajectoryPoint(const TrajectoryPoint& other)
{
  *this = other;
}



const TrajectoryPoint& TrajectoryPoint::operator=(const TrajectoryPoint& other)
{
  _position = other._position;
  _time     = other._time;

  return *this;
}



TrajectoryPoint::~TrajectoryPoint()
{
}



