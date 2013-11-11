// ----------------------------------------------------------------------------
///  \file   IonizationHit.h
///  \brief  Ionization deposit left by a particle in an active volume.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     27 Apr 2009
///  \version  $Id$ 
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __IONIZATION_HIT__
#define __IONIZATION_HIT__

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>



namespace nexus {

  /// Ionization deposit left by a particle in an active volume

  class IonizationHit: public G4VHit
  {
  public:
    /// Constructor
    IonizationHit();
    /// Copy constructor
    IonizationHit(const IonizationHit&);
    /// Destructor
    virtual ~IonizationHit();
    
    /// Assignement operator
    const IonizationHit& operator=(const IonizationHit&);
    /// Equality operator
    G4int operator==(const IonizationHit&) const;
    /// Memory allocation
    void* operator new(size_t);
    /// Memory deallocation
    void operator delete(void*);

  public:
    G4int GetTrackID();
    void SetTrackID(G4int);
    
    G4double GetTime();
    void SetTime(G4double);

    G4double GetEnergyDeposit();
    void SetEnergyDeposit(G4double);

    G4ThreeVector GetPosition();
    void SetPosition(G4ThreeVector);

  private:
    G4int _track_id;         
    G4double _time;
    G4double _energy_dep;
    G4ThreeVector _position; 
  };
  
  
  typedef G4THitsCollection<IonizationHit> IonizationHitsCollection;
  extern G4Allocator<IonizationHit> IonizationHitAllocator;

  
  // INLINE DEFINITIONS //////////////////////////////////////////////
  
  inline void* IonizationHit::operator new(size_t) 
  { return ((void*) IonizationHitAllocator.MallocSingle()); }
  
  inline void IonizationHit::operator delete(void* aHit)
  { IonizationHitAllocator.FreeSingle((IonizationHit*) aHit); }

  inline G4int IonizationHit::GetTrackID() { return _track_id; }
  inline void IonizationHit::SetTrackID(G4int id) { _track_id = id; }
  
  inline G4double IonizationHit::GetTime() { return _time; }
  inline void IonizationHit::SetTime(G4double t) { _time = t; }

  inline G4double IonizationHit::GetEnergyDeposit() { return _energy_dep; }
  inline void IonizationHit::SetEnergyDeposit(G4double edep) 
  { _energy_dep = edep; }

  inline G4ThreeVector IonizationHit::GetPosition() { return _position; }
  inline void IonizationHit::SetPosition(G4ThreeVector xyz) 
  { _position = xyz; }


} // end namespace nexus

#endif
