// ----------------------------------------------------------------------------
///  \file   TrajectoryPoint.h
///  \brief  A point (position, time) in the trajectory of a track
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     27 March 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef TRAJECTORY_POINT_H
#define TRAJECTORY_POINT_H

#include <G4VTrajectoryPoint.hh>
#include <G4Allocator.hh>


namespace nexus {

  /// A point (position and time) in the trajectory of a simulated track

  class TrajectoryPoint: public G4VTrajectoryPoint
  {
  public:
    /// Default constructor
    TrajectoryPoint();
    /// Constructor giving a position and time
    TrajectoryPoint(G4ThreeVector, G4double);
    /// Copy constructor
    TrajectoryPoint(const TrajectoryPoint&);
    /// Destructor
    virtual ~TrajectoryPoint();

    /// Assignement operator
    const TrajectoryPoint& operator =(const TrajectoryPoint&);
    /// Equality operator
    int operator ==(const TrajectoryPoint&) const;
    /// Memory allocation
    void* operator new(size_t);
    /// Memory deallocation
    void operator delete(void*);

    /// Return the (global) position of the point
    const G4ThreeVector GetPosition() const;
    /// Return the (global) time of the point
    G4double GetTime() const;

  private:
    G4ThreeVector _position;
    G4double _time;
};

} // namespace nexus

#if defined G4TRACKING_ALLOC_EXPORT
extern G4DLLEXPORT G4Allocator<nexus::TrajectoryPoint> TrjPointAllocator;
#else
extern G4DLLIMPORT G4Allocator<nexus::TrajectoryPoint> TrjPointAllocator;
#endif

// INLINE DEFINITIONS //////////////////////////////////////

namespace nexus {

  inline int TrajectoryPoint::operator ==(const TrajectoryPoint& other) const
  {return (this==&other); }

  inline void* TrajectoryPoint::operator new(size_t)
  { return ((void*) TrjPointAllocator.MallocSingle()); }

  inline void TrajectoryPoint::operator delete(void* tp)
  { TrjPointAllocator.FreeSingle((TrajectoryPoint*) tp); }

  inline const G4ThreeVector TrajectoryPoint::GetPosition() const
  { return _position; }

  inline G4double TrajectoryPoint::GetTime() const { return _time; }

} // end namespace nexus

#endif