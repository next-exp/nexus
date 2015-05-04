// ----------------------------------------------------------------------------
///  \file   TrajectoryMap.h
///  \brief  
///  
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     25 Mar 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __TRAJECTORY_MAP__
#define __TRAJECTORY_MAP__

#include <map>

class G4VTrajectory;


namespace nexus {

  class TrajectoryMap
  {
  public:
    /// Return a trajectory given its track ID
    static G4VTrajectory* Get(int trackId);
    /// Add a trajectory to the map
    static void Add(G4VTrajectory*);
    /// Clear the map
    static void Clear();

  private:
    // Constructors, destructor and assignement op are hidden
    // so that no instance of the class can be created.
    TrajectoryMap();
    TrajectoryMap(const TrajectoryMap&);
    ~TrajectoryMap();

  private:
    static std::map<int, G4VTrajectory*> _map;
  };

} // namespace nexus

#endif  

