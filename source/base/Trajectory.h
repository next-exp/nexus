// ----------------------------------------------------------------------------
///  \file   Trajectory.h
///  \brief  
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     26 March 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __TRAJECTORY__
#define __TRAJECTORY__

#include <G4VTrajectory.hh>
#include <G4Allocator.hh>

class G4Track;
class G4ParticleDefinition;
class G4VTrajectoryPoint;


namespace nexus {

  typedef std::vector<G4VTrajectoryPoint*> TrajectoryPointContainer;


  /// TODO. CLASS DESCRIPTION

  class Trajectory: public G4VTrajectory
  {
  public:
    /// Default constructor
    Trajectory();
    /// Constructor given a track
    Trajectory(const G4Track*);
    /// Copy constructor
    Trajectory(const Trajectory&);
    /// Destructor
    virtual ~Trajectory();

    /// Memory allocation operator
    void* operator new(size_t);
    /// Memory deallocation operator
    void operator delete(void*);
    /// Equality operator
    int operator ==(const Trajectory&) const;

  public:

    G4ParticleDefinition* GetParticleDefinition();
    void SetParticleDefinition(G4ParticleDefinition*);

    G4String GetParticleName() const;
    G4double GetCharge() const;
    G4int GetPDGEncoding () const;
  
    G4int GetTrackID() const;
    void SetTrackID(G4int);

    G4int GetParentID() const;
    void SetParentID(G4int);

    G4ThreeVector GetInitialMomentum() const;
    void SetInitialMomentum(const G4ThreeVector&);
    
    G4ThreeVector GetInitialPosition() const;
    void SetInitialPosition(const G4ThreeVector&);

    G4ThreeVector GetFinalPosition() const;
    void SetFinalPosition(const G4ThreeVector&);

    G4double GetInitialTime() const;
    void SetInitialTime(G4double);

    G4double GetFinalTime() const;
    void SetFinalTime(G4double);

    
    // Trajectory points

    /// Return the number of trajectory points
    virtual int GetPointEntries() const;
    /// Return the i-th point in the trajectory
    virtual G4VTrajectoryPoint* GetPoint(G4int i) const;
    ///
    virtual void AppendStep(const G4Step*);
    ///
    virtual void MergeTrajectory(G4VTrajectory*);


  private:
    G4ParticleDefinition* _pdef; //< Pointer to the particle definition

    G4int _trackId;   ///< Identification number of the track
    G4int _parentId;  ///< Identification number of the parent particle

    G4ThreeVector _initial_momentum;

    G4ThreeVector _initial_position;
    G4ThreeVector _final_position;

    G4double _initial_time;
    G4double _final_time;

    G4bool _record_trjpoints;

    TrajectoryPointContainer* _trjpoints;
  };

} // namespace nexus


#if defined G4TRACKING_ALLOC_EXPORT
  extern G4DLLEXPORT G4Allocator<nexus::Trajectory> TrjAllocator;
#else
  extern G4DLLIMPORT G4Allocator<nexus::Trajectory> TrjAllocator;
#endif


// INLINE DEFINITIONS //////////////////////////////////////////////

inline void* nexus::Trajectory::operator new(size_t)
{ return ((void*) TrjAllocator.MallocSingle()); }

inline void nexus::Trajectory::operator delete(void* trj)
{ TrjAllocator.FreeSingle((nexus::Trajectory*) trj); }

inline int nexus::Trajectory::GetPointEntries() const
{ return _trjpoints->size(); }

inline G4VTrajectoryPoint* nexus::Trajectory::GetPoint(G4int i) const
{ return (*_trjpoints)[i]; }

inline G4ThreeVector nexus::Trajectory::GetInitialMomentum() const
{ return _initial_momentum; }

inline void nexus::Trajectory::SetInitialMomentum(const G4ThreeVector& p)
{ _initial_momentum = p; }

inline G4int nexus::Trajectory::GetTrackID() const
{ return _trackId; }

inline void nexus::Trajectory::SetTrackID(G4int id)
{ _trackId = id; }

inline G4int nexus::Trajectory::GetParentID() const
{ return _parentId; }

inline void nexus::Trajectory::SetParentID(G4int id)
{ _parentId = id; }

inline G4ThreeVector nexus::Trajectory::GetInitialPosition() const
{ return _initial_position; }

inline void nexus::Trajectory::SetInitialPosition(const G4ThreeVector& x)
{ _initial_position = x; }

inline G4ThreeVector nexus::Trajectory::GetFinalPosition() const
{ return _final_position; }

inline void nexus::Trajectory::SetFinalPosition(const G4ThreeVector& x)
{ _final_position = x; }

inline G4double nexus::Trajectory::GetInitialTime() const
{ return _initial_time; }

inline void nexus::Trajectory::SetInitialTime(G4double t)
{ _initial_time = t; }

inline G4double nexus::Trajectory::GetFinalTime() const
{ return _final_time; }

inline void nexus::Trajectory::SetFinalTime(G4double t)
{ _final_time = t; }

#endif  