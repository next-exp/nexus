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

    // Getters -- Particle properties
    G4ParticleDefinition* GetParticleDefinition();
    G4int GetTrackID() const;
    G4int GetParentID() const;
    G4String GetParticleName() const;
    G4double GetCharge() const;
    G4int GetPDGEncoding() const;
    G4ThreeVector GetInitialMomentum() const;
    
    
    // Trajectory points

    /// Return the number of trajectory points
    virtual int GetPointEntries() const;
    /// Return the i-th point in the trajectory
    virtual G4VTrajectoryPoint* GetPoint(G4int i) const;
    virtual void AppendStep(const G4Step*);
    virtual void MergeTrajectory(G4VTrajectory*);

    //virtual void ShowTrajectory(std::ostream& os=G4cout) const;
    //virtual void DrawTrajectory(G4int i_mode=0) const;
    //virtual const std::map<G4String, G4AttDef>* GetAttDefs() const;
    //virtual std::vector<G4AttValue>* CreateAttValues() const;

  private:
    G4ParticleDefinition* _pdef;

    G4int _trackId;   ///< Identification number of the track
    G4int _parentId;  ///< Identification number of the parent particle

    G4ThreeVector _initial_momentum;
    G4ThreeVector _initial_position;

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


#endif  