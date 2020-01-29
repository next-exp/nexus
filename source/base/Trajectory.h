// ----------------------------------------------------------------------------
///  \file   Trajectory.h
///  \brief  Record of a track used by the persistency mechanism.
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


  /// Record of a track used by the persistency mechanism.
  /// It stores basic information of the particle and its path through
  /// the geometry. A number of its properties are copied from the track
  /// in construction and cannot be modified.

  class Trajectory: public G4VTrajectory
  {
  public:
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

    /// Return pointer to the particle definition
    /// associated to the track
    G4ParticleDefinition* GetParticleDefinition();
    /// Return name of the particle
    G4String GetParticleName() const;
    /// Return charge of the particle
    G4double GetCharge() const;
    /// Return PDG code of the particle
    G4int GetPDGEncoding () const;

    // Return name of the track creator process
    G4String GetCreatorProcess() const;

    /// Return id number of the associated track
    G4int GetTrackID() const;
    /// Return id number of the parent track
    G4int GetParentID() const;

    // Return initial three-momentum
    G4ThreeVector GetInitialMomentum() const;
    // Return initial position (creation vertex)
    // in global coordinates
    G4ThreeVector GetInitialPosition() const;
    // Return creation time with respect to
    // the start-of-event time
    G4double GetInitialTime() const;

    G4ThreeVector GetFinalMomentum() const;
    void SetFinalMomentum(const G4ThreeVector&);

    G4ThreeVector GetFinalPosition() const;
    void SetFinalPosition(const G4ThreeVector&);

    G4double GetFinalTime() const;
    void SetFinalTime(G4double);

    G4double GetTrackLength() const;
    void SetTrackLength(G4double);

    G4double GetEnergyDeposit() const;
    void SetEnergyDeposit(G4double);

    G4String GetInitialVolume() const;

    G4String GetFinalVolume() const;
    void SetFinalVolume(G4String);

    // Return name of the track killer process
    G4String GetFinalProcess() const;
    void SetFinalProcess(G4String);


    // Trajectory points

    /// Return the number of trajectory points
    virtual int GetPointEntries() const;
    /// Return the i-th point in the trajectory
    virtual G4VTrajectoryPoint* GetPoint(G4int i) const;
    ///
    virtual void AppendStep(const G4Step*);
    ///
    virtual void MergeTrajectory(G4VTrajectory*);

    virtual void ShowTrajectory(std::ostream&) const;

    virtual void DrawTrajectory() const;

  private:
    /// The default constructor is private. A trajectory can
    /// only be constructed associated to a track.
    Trajectory();


  private:
    G4ParticleDefinition* _pdef; //< Pointer to the particle definition

    G4int _trackId;   ///< Identification number of the track
    G4int _parentId;  ///< Identification number of the parent particle

    G4ThreeVector _initial_momentum;
    G4ThreeVector _final_momentum;

    G4ThreeVector _initial_position;
    G4ThreeVector _final_position;

    G4double _initial_time;
    G4double _final_time;

    G4double _length;
    G4double _edep;

    G4String _creator_process;
    G4String _final_process;

    G4String _initial_volume;
    G4String _final_volume;

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

inline G4ParticleDefinition* nexus::Trajectory::GetParticleDefinition()
{ return _pdef; }

inline int nexus::Trajectory::GetPointEntries() const
{ return _trjpoints->size(); }

inline G4VTrajectoryPoint* nexus::Trajectory::GetPoint(G4int i) const
{ return (*_trjpoints)[i]; }

inline G4ThreeVector nexus::Trajectory::GetInitialMomentum() const
{ return _initial_momentum; }

inline G4int nexus::Trajectory::GetTrackID() const
{ return _trackId; }

inline G4int nexus::Trajectory::GetParentID() const
{ return _parentId; }

inline G4ThreeVector nexus::Trajectory::GetFinalMomentum() const
{ return _final_momentum; }

inline void nexus::Trajectory::SetFinalMomentum(const G4ThreeVector& m)
{ _final_momentum = m; }

inline G4ThreeVector nexus::Trajectory::GetInitialPosition() const
{ return _initial_position; }

inline G4ThreeVector nexus::Trajectory::GetFinalPosition() const
{ return _final_position; }

inline void nexus::Trajectory::SetFinalPosition(const G4ThreeVector& x)
{ _final_position = x; }

inline G4double nexus::Trajectory::GetInitialTime() const
{ return _initial_time; }

inline G4double nexus::Trajectory::GetFinalTime() const
{ return _final_time; }

inline void nexus::Trajectory::SetFinalTime(G4double t)
{ _final_time = t; }

inline G4double nexus::Trajectory::GetTrackLength() const { return _length; }

inline void nexus::Trajectory::SetTrackLength(G4double l) { _length = l; }

inline G4double nexus::Trajectory::GetEnergyDeposit() const { return _edep; }

inline void nexus::Trajectory::SetEnergyDeposit(G4double e) { _edep = e; }

inline G4String nexus::Trajectory::GetCreatorProcess() const
{ return _creator_process; }

inline G4String nexus::Trajectory::GetFinalProcess() const
{ return _final_process; }

inline void nexus::Trajectory::SetFinalProcess(G4String fp)
{ _final_process = fp; }

inline G4String nexus::Trajectory::GetInitialVolume() const
{ return _initial_volume; }

inline G4String nexus::Trajectory::GetFinalVolume() const
{ return _final_volume; }

inline void nexus::Trajectory::SetFinalVolume(G4String fv)
{ _final_volume = fv; }

#endif
