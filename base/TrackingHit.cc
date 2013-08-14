//
//  TrackingHit.cc
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>
//     Created: 27 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- IFIC Neutrino Group 
//

#include "TrackingHit.h"


namespace nexus {


  G4Allocator<TrackingHit> TrackingHitAllocator;



  TrackingHit::TrackingHit(const TrackingHit& right): G4VHit()
  {
    _track_id   = right._track_id;
    _energy_dep = right._energy_dep;
    _position   = right._position;
  }
  


  const TrackingHit& TrackingHit::operator=(const TrackingHit& right)
  {
    _track_id   = right._track_id;
    _energy_dep = right._energy_dep;
    _position   = right._position;
    return *this;
  }



  G4int TrackingHit::operator==(const TrackingHit& right) const
  {
    return (this==&right) ? 1 : 0;
  }
  
  
  
//   void TrackingHit::Print()
//   {
//   }
  

} // end namespace nexus
