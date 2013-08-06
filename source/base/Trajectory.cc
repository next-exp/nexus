// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 26 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "Trajectory.h"

#include "TrajectoryPoint.h"

#include <G4Track.hh>
#include <G4ParticleDefinition.hh>

using namespace nexus;



G4Allocator<Trajectory> TrjAllocator;



Trajectory::Trajectory():
  G4VTrajectory(), _pdef(0), _trackId(-1), _parentId(-1),
  _initial_momentum(), _record_trjpoints(true)
{
  _trjpoints = new TrajectoryPointContainer();
}



Trajectory::Trajectory(const G4Track* track)
{
  _record_trjpoints = true;
  _pdef = track->GetDefinition();
  _trackId = track->GetTrackID();
  _parentId = track->GetParentID();
  _initial_momentum = track->GetMomentum();
  _trjpoints = new TrajectoryPointContainer();
}



Trajectory::Trajectory(const Trajectory& other): G4VTrajectory()
{
  _pdef = other._pdef;
}



Trajectory::~Trajectory()
{
  for (unsigned int i=0; i<_trjpoints->size(); ++i) 
    delete (*_trjpoints)[i];
  _trjpoints->clear();
  delete _trjpoints;
}



G4ParticleDefinition* Trajectory::GetParticleDefinition()
{
  return _pdef;
}



void Trajectory::SetParticleDefinition(G4ParticleDefinition* pdef)
{
  _pdef = pdef;
}



G4String Trajectory::GetParticleName() const
{
  return _pdef->GetParticleName();
}



G4int Trajectory::GetPDGEncoding() const
{
  return _pdef->GetPDGEncoding();
}



G4double Trajectory::GetCharge() const
{
  return _pdef->GetPDGCharge();
}



void Trajectory::AppendStep(const G4Step* aStep)
{
  if (!_record_trjpoints) return;

  TrajectoryPoint* point = new TrajectoryPoint();
  _trjpoints->push_back(point);
}



void Trajectory::MergeTrajectory(G4VTrajectory* second)
{
  if (!second) return;

  Trajectory* tmp = (Trajectory*) second;
  G4int entries = tmp->GetPointEntries();

  // initial point of the second trajectory should not be merged
  for (G4int i=1; i<entries ; ++i) { 
    _trjpoints->push_back((*(tmp->_trjpoints))[i]);
  }

  delete (*tmp->_trjpoints)[0];
  tmp->_trjpoints->clear();
}

 