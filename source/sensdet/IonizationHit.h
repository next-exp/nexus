// ----------------------------------------------------------------------------
// nexus | IonizationHit.h
//
// This class describes the ionization deposit left by a particle
// in a sensitive volume.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef IONIZATION_HIT_H
#define IONIZATION_HIT_H

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
    G4int track_id_;
    G4double time_;
    G4double energy_dep_;
    G4ThreeVector position_;
  };


  typedef G4THitsCollection<IonizationHit> IonizationHitsCollection;
  extern G4Allocator<IonizationHit> IonizationHitAllocator;


  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline void* IonizationHit::operator new(size_t)
  { return ((void*) IonizationHitAllocator.MallocSingle()); }

  inline void IonizationHit::operator delete(void* aHit)
  { IonizationHitAllocator.FreeSingle((IonizationHit*) aHit); }

  inline G4int IonizationHit::GetTrackID() { return track_id_; }
  inline void IonizationHit::SetTrackID(G4int id) { track_id_ = id; }

  inline G4double IonizationHit::GetTime() { return time_; }
  inline void IonizationHit::SetTime(G4double t) { time_ = t; }

  inline G4double IonizationHit::GetEnergyDeposit() { return energy_dep_; }
  inline void IonizationHit::SetEnergyDeposit(G4double edep)
  { energy_dep_ = edep; }

  inline G4ThreeVector IonizationHit::GetPosition() { return position_; }
  inline void IonizationHit::SetPosition(G4ThreeVector xyz)
  { position_ = xyz; }


} // end namespace nexus

#endif
