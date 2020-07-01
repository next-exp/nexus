// ----------------------------------------------------------------------------
// nexus | Trajectory.cc
//
// This class records the relevant information of a particle and its path
// through the geometry. It is later used by the persistency mechanism to write
// the particle information in the output file.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Trajectory.h"

#include "TrajectoryPoint.h"
#include "TrajectoryMap.h"

#include <G4Track.hh>
#include <G4ParticleDefinition.hh>
#include <G4VProcess.hh>

using namespace nexus;



G4Allocator<Trajectory> TrjAllocator;



Trajectory::Trajectory(const G4Track* track): 
  G4VTrajectory(), pdef_(0), trackId_(-1), parentId_(-1),
  initial_time_(0.), final_time_(0), length_(0.), edep_(0.), 
  record_trjpoints_(true), trjpoints_(0)
{
  pdef_     = track->GetDefinition();
  trackId_  = track->GetTrackID();
  parentId_ = track->GetParentID();

  if (parentId_ == 0) 
    creator_process_ = "none";
  else 
    creator_process_ = track->GetCreatorProcess()->GetProcessName();

  initial_momentum_ = track->GetMomentum();
  initial_position_ = track->GetVertexPosition();
  initial_time_ = track->GetGlobalTime();
  initial_volume_ = track->GetVolume()->GetName();

  trjpoints_ = new TrajectoryPointContainer();

  // Add this trajectory in the map, but only if no other
  // trajectory for this track id has been registered yet
  if (!TrajectoryMap::Get(track->GetTrackID()))
    TrajectoryMap::Add(this);
}



Trajectory::Trajectory(const Trajectory& other): G4VTrajectory()
{
  pdef_ = other.pdef_;
}



Trajectory::~Trajectory()
{
  for (unsigned int i=0; i<trjpoints_->size(); ++i) 
    delete (*trjpoints_)[i];
  trjpoints_->clear();
  delete trjpoints_;
}



G4String Trajectory::GetParticleName() const
{
  return pdef_->GetParticleName();
}



G4int Trajectory::GetPDGEncoding() const
{
  return pdef_->GetPDGEncoding();
}



G4double Trajectory::GetCharge() const
{
  return pdef_->GetPDGCharge();
}



void Trajectory::AppendStep(const G4Step* step)
{
  if (!record_trjpoints_) return;

  TrajectoryPoint* point = 
    new TrajectoryPoint(step->GetPostStepPoint()->GetPosition(),
                        step->GetPostStepPoint()->GetGlobalTime());
  trjpoints_->push_back(point);
}



void Trajectory::MergeTrajectory(G4VTrajectory* second)
{
  if (!second) return;

  if (!record_trjpoints_) return;

  Trajectory* tmp = (Trajectory*) second;
  G4int entries = tmp->GetPointEntries();

  // initial point of the second trajectory should not be merged
  for (G4int i=1; i<entries ; ++i) { 
    trjpoints_->push_back((*(tmp->trjpoints_))[i]);
  }

  delete (*tmp->trjpoints_)[0];
  tmp->trjpoints_->clear();
}



void Trajectory::ShowTrajectory(std::ostream& os) const
{
  // Invoke the default implementation
  G4VTrajectory::ShowTrajectory(os);
}
 


 void Trajectory::DrawTrajectory() const
 {
  // Invoke the default implementation
  G4VTrajectory::DrawTrajectory();
 }

