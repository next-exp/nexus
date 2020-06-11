// ----------------------------------------------------------------------------
// nexus | IonizationHit.cc
//
// This class describes the ionization deposit left by a particle
// in a sensitive volume.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "IonizationHit.h"



namespace nexus {


  G4Allocator<IonizationHit> IonizationHitAllocator;



  IonizationHit::IonizationHit(): G4VHit()
  {
  }



  IonizationHit::IonizationHit(const IonizationHit& other): G4VHit()
  {
    *this = other;
  }



  IonizationHit::~IonizationHit()
  {
  }



  const IonizationHit& IonizationHit::operator=(const IonizationHit& other)
  {
    track_id_   = other.track_id_;
    time_       = other.time_;
    energy_dep_ = other.energy_dep_;
    position_   = other.position_;

    return *this;
  }



  G4int IonizationHit::operator==(const IonizationHit& other) const
  {
    return (this==&other) ? 1 : 0;
  }


} // end namespace nexus
