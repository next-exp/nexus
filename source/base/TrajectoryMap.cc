// ----------------------------------------------------------------------------
// nexus | TrajectoryMap.cc
//
// This class is a container of particle trajectories.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "TrajectoryMap.h"

#include <G4VTrajectory.hh>


std::map<int, G4VTrajectory*> nexus::TrajectoryMap::map_;


namespace nexus {

  TrajectoryMap::TrajectoryMap()
  {
  }



  TrajectoryMap::~TrajectoryMap()
  {
    map_.clear();
  }



  void TrajectoryMap::Clear()
  {
    map_.clear();
  }



  G4VTrajectory* TrajectoryMap::Get(int trackId)
  {
    std::map<int, G4VTrajectory*>::iterator it = map_.find(trackId);
    if (it == map_.end()) return 0;
    else return it->second;
  }



  void TrajectoryMap::Add(G4VTrajectory* trj)
  {
    map_[trj->GetTrackID()] = trj;
  }

} // namespace nexus
