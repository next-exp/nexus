// ----------------------------------------------------------------------------
///  \file   IonizationHit.h
///  \brief  Ionization deposit left by a particle in an active volume.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>
///  \date     27 Apr 2009
///  \version  $Id$ 
///
///  Copyright (c) 2009 NEXT Collaboration
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
    G4int GetTrackID();
    void SetTrackID(G4int);
    
    G4double GetTime();
    void SetTime(G4double);

    G4double GetEnergyDeposit();
    void SetEnergyDeposit(G4double);

    G4ThreeVector GetPosition();
    void SetPosition(G4ThreeVector);

//    bhep::hit* ToBhep(const G4String& det_name) const;

  public:
    /// Constructor
    IonizationHit();
    /// Destructor
    ~IonizationHit();
    /// Copy-constructor
    IonizationHit(const IonizationHit&);
    /// Assignement operator
    const IonizationHit& operator =(const IonizationHit&);
    /// Equality operator
    G4int operator ==(const IonizationHit&) const;
    
    /// memory allocation
    void* operator new(size_t);
    /// memory deallocation
    inline void operator delete(void*);
    
  private:
    G4int _track_id;         
    G4double _time;
    G4double _energy_dep;
    G4ThreeVector _position; 
  };
  
  
  typedef G4THitsCollection<IonizationHit> IonizationHitsCollection;
  
  extern G4Allocator<IonizationHit> IonizationHitAllocator;

  
  // inline methods ........................................
  
  // memory management
  
  inline void* IonizationHit::operator new(size_t) 
  { void* aHit;
    aHit = (void*) IonizationHitAllocator.MallocSingle();
    return aHit; }
  
  inline void IonizationHit::operator delete(void* aHit)
  { IonizationHitAllocator.FreeSingle((IonizationHit*) aHit); }

  // setters & getters

  inline G4int IonizationHit::GetTrackID() { return _track_id; }
  inline void IonizationHit::SetTrackID(G4int id) { _track_id = id; }
  
  inline G4double IonizationHit::GetTime() { return _time; }
  inline void IonizationHit::SetTime(G4double t) { _time = t; }

  inline G4double IonizationHit::GetEnergyDeposit() { return _energy_dep; }
  inline void IonizationHit::SetEnergyDeposit(G4double edep) 
  { _energy_dep = edep; }

  inline G4ThreeVector IonizationHit::GetPosition() { return _position; }
  inline void IonizationHit::SetPosition(G4ThreeVector xyz) { _position = xyz; }


} // end namespace nexus

#endif
