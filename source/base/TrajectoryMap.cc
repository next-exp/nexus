// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 25 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "TrajectoryMap.h"

#include <G4VTrajectory.hh>


std::map<int, G4VTrajectory*> nexus::TrajectoryMap::_map;


namespace nexus {

  TrajectoryMap::TrajectoryMap()
  {
  }



  TrajectoryMap::~TrajectoryMap()
  {
    _map.clear();
  }



  void TrajectoryMap::Clear()
  {
    _map.clear();
  }



  G4VTrajectory* TrajectoryMap::Get(int trackId)
  {
    std::map<int, G4VTrajectory*>::iterator it = _map.find(trackId);
    if (it == _map.end()) return 0;
    else return it->second;
  }



  void TrajectoryMap::Add(G4VTrajectory* trj)
  {
    _map[trj->GetTrackID()] = trj;
  }

} // namespace nexus
