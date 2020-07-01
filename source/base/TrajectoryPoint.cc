// ----------------------------------------------------------------------------
// nexus | TrajectoryPoint.cc
//
// This class describes a point (position and time) in the trajectory
// of a particle.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "TrajectoryPoint.h"

using namespace nexus;


G4Allocator<TrajectoryPoint> TrjPointAllocator;


TrajectoryPoint::TrajectoryPoint():
  position_(0.,0.,0.), time_(0.)
{
}



TrajectoryPoint::TrajectoryPoint(G4ThreeVector pos, G4double t):
  position_(pos), time_(t)
{
}



TrajectoryPoint::TrajectoryPoint(const TrajectoryPoint& other)
{
  *this = other;
}



const TrajectoryPoint& TrajectoryPoint::operator=(const TrajectoryPoint& other)
{
  position_ = other.position_;
  time_     = other.time_;

  return *this;
}



TrajectoryPoint::~TrajectoryPoint()
{
}
