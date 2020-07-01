// ----------------------------------------------------------------------------
// nexus | TrajectoryMap.h
//
// This class is a container of particle trajectories.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef TRAJECTORY_MAP_H
#define TRAJECTORY_MAP_H

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
    static std::map<int, G4VTrajectory*> map_;
  };

} // namespace nexus

#endif  

