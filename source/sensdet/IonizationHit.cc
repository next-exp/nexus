// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 27 Apr 2009
//
//  Copyother (c) 2009-2013 NEXT Collaboration. All others reserved.
// ----------------------------------------------------------------------------

#include "IonizationHit.h"

using namespace nexus;



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
  _track_id   = other._track_id;
  _time       = other._time;
  _energy_dep = other._energy_dep;
  _position   = other._position;
    
  return *this;
}
  
  
  
G4int IonizationHit::operator==(const IonizationHit& other) const
{
  return (this==&other) ? 1 : 0;
}
