//
//  TrackingHit.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>
//     Created: 27 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- IFIC Neutrino Group 
//

#ifndef __TRACKING_HIT__
#define __TRACKING_HIT__

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>


namespace nexus {

  class TrackingHit: public G4VHit
  {
  public:
    /// Constructor
    TrackingHit() {}
    /// Destructor
    ~TrackingHit() {}
    /// Copy-constructor
    TrackingHit(const TrackingHit&);
    /// Assignement operator
    const TrackingHit& operator=(const TrackingHit&);
    /// Equality operator
    G4int operator==(const TrackingHit&) const;
    
    inline void* operator new(size_t);
    inline void operator delete(void*);
    
    //void Print();
 
    /// Setters/Getters
    ///
    inline void SetTrackID(G4int id) 
    { _track_id = id; }
    inline G4int GetTrackID()
    { return _track_id; }
    ///
    inline void SetEnergyDeposit(G4double edep)
    { _energy_dep = edep; }
    inline G4double GetEnergyDeposit()
    { return _energy_dep; }
    ///
    void SetPosition(G4ThreeVector xyz)
    { _position = xyz; }
    G4ThreeVector GetPosition()
    { return _position; }

  private:
    G4int _track_id;
    G4double _energy_dep;
    G4ThreeVector _position;
  };
  
  
  typedef G4THitsCollection<TrackingHit> TrackingHitsCollection;
  
  extern G4Allocator<TrackingHit> TrackingHitAllocator;
  
  
  inline void* TrackingHit::operator new(size_t)
  {
    void* aHit;
    aHit = (void*) TrackingHitAllocator.MallocSingle();
    return aHit;
  }

  inline void TrackingHit::operator delete(void* aHit)
  {
    TrackingHitAllocator.FreeSingle((TrackingHit*) aHit);
  }

} // end namespace nexus

#endif
