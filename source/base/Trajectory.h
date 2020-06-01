// ----------------------------------------------------------------------------
// nexus | Trajectory.h
//
// This class records the relevant information of a particle and its path
// through the geometry. It is later used by the persistency mechanism to write
// the particle information in the output file.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <G4VTrajectory.hh>
#include <G4Allocator.hh>

class G4Track;
class G4ParticleDefinition;
class G4VTrajectoryPoint;


namespace nexus {

  typedef std::vector<G4VTrajectoryPoint*> TrajectoryPointContainer;

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
    G4ParticleDefinition* pdef_; //< Pointer to the particle definition

    G4int trackId_;   ///< Identification number of the track
    G4int parentId_;  ///< Identification number of the parent particle

    G4ThreeVector initial_momentum_;
    G4ThreeVector final_momentum_;

    G4ThreeVector initial_position_;
    G4ThreeVector final_position_;

    G4double initial_time_;
    G4double final_time_;

    G4double length_;
    G4double edep_;

    G4String creator_process_;
    G4String final_process_;

    G4String initial_volume_;
    G4String final_volume_;

    G4bool record_trjpoints_;

    TrajectoryPointContainer* trjpoints_;

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
{ return pdef_; }

inline int nexus::Trajectory::GetPointEntries() const
{ return trjpoints_->size(); }

inline G4VTrajectoryPoint* nexus::Trajectory::GetPoint(G4int i) const
{ return (*trjpoints_)[i]; }

inline G4ThreeVector nexus::Trajectory::GetInitialMomentum() const
{ return initial_momentum_; }

inline G4int nexus::Trajectory::GetTrackID() const
{ return trackId_; }

inline G4int nexus::Trajectory::GetParentID() const
{ return parentId_; }

inline G4ThreeVector nexus::Trajectory::GetFinalMomentum() const
{ return final_momentum_; }

inline void nexus::Trajectory::SetFinalMomentum(const G4ThreeVector& m)
{ final_momentum_ = m; }

inline G4ThreeVector nexus::Trajectory::GetInitialPosition() const
{ return initial_position_; }

inline G4ThreeVector nexus::Trajectory::GetFinalPosition() const
{ return final_position_; }

inline void nexus::Trajectory::SetFinalPosition(const G4ThreeVector& x)
{ final_position_ = x; }

inline G4double nexus::Trajectory::GetInitialTime() const
{ return initial_time_; }

inline G4double nexus::Trajectory::GetFinalTime() const
{ return final_time_; }

inline void nexus::Trajectory::SetFinalTime(G4double t)
{ final_time_ = t; }

inline G4double nexus::Trajectory::GetTrackLength() const { return length_; }

inline void nexus::Trajectory::SetTrackLength(G4double l) { length_ = l; }

inline G4double nexus::Trajectory::GetEnergyDeposit() const { return edep_; }

inline void nexus::Trajectory::SetEnergyDeposit(G4double e) { edep_ = e; }

inline G4String nexus::Trajectory::GetCreatorProcess() const
{ return creator_process_; }

inline G4String nexus::Trajectory::GetFinalProcess() const
{ return final_process_; }

inline void nexus::Trajectory::SetFinalProcess(G4String fp)
{ final_process_ = fp; }

inline G4String nexus::Trajectory::GetInitialVolume() const
{ return initial_volume_; }

inline G4String nexus::Trajectory::GetFinalVolume() const
{ return final_volume_; }

inline void nexus::Trajectory::SetFinalVolume(G4String fv)
{ final_volume_ = fv; }

#endif
