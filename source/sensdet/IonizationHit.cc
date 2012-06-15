// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>
//  Created: 27 Apr 2009
//
//  Copyright (c) 2009 NEXT Collaboration 
// ----------------------------------------------------------------------------

#include "IonizationHit.h"

#include "BhepUtils.h"

#include <bhep/hit.h>
#include <bhep/point.h>


namespace nexus {


  G4Allocator<IonizationHit> IonizationHitAllocator;
  

  
  IonizationHit::IonizationHit()
  {
  }

  
  
  IonizationHit::IonizationHit(const IonizationHit& right): G4VHit()
  {
    *this = right;
  }
  


  IonizationHit::~IonizationHit()
  {
  }
  
  
  
  const IonizationHit& IonizationHit::operator =(const IonizationHit& right)
  {
    _track_id   = right._track_id;
    _time       = right._time;
    _energy_dep = right._energy_dep;
    _position   = right._position;
    
    return *this;
  }
  
  
  
  G4int IonizationHit::operator ==(const IonizationHit& right) const
  {
    return (this==&right) ? 1 : 0;
  }
  
  
  
  bhep::hit* IonizationHit::ToBhep(const G4String& det_name) const
  {
    // create a bhep hit
    bhep::hit* bhit = new bhep::hit(bhep::TRUTH, det_name);

    // set properties
    bhit->set_point(bhep::Point3D(_position.x(),
				  _position.y(),
				  _position.z()));

    bhit->add_property("Time", _time);

    bhit->add_property("EnergyDep", _energy_dep);

    // associate hit to particle
    bhep::particle& bpart = BhepUtils::GetBParticle(_track_id);
    bhit->set_mother_particle(bpart);
    bpart.add_hit(det_name, bhit);
    
    return bhit;
  }
  

} // end namespace nexus
