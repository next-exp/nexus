// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 26 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "Trajectory.h"

#include <G4Track.hh>
#include <G4ParticleDefinition.hh>





using namespace nexus;


G4Allocator<Trajectory> TrjAllocator;



Trajectory::Trajectory():
  G4VTrajectory(), _pdef(0), _trackId(-1), _parentId(-1)
{
}



Trajectory::Trajectory(const G4Track* track)
{
  _pdef     = track->GetDefinition();
  _trackId  = track->GetTrackID();
  _parentId = track->GetParentID();
}



Trajectory::Trajectory(const Trajectory& other): G4VTrajectory()
{
  _pdef = other._pdef;
}



Trajectory::~Trajectory()
{
}



G4ParticleDefinition* Trajectory::GetParticleDefinition()
{
  return _pdef;
}



G4int Trajectory::GetTrackID() const
{
  return _trackId;
}



G4int Trajectory::GetParentID() const
{
  return _parentId;
}



G4String Trajectory::GetParticleName() const
{
  return _pdef->GetParticleName();
}



G4double Trajectory::GetCharge() const
{
  return _pdef->GetPDGCharge();
}



G4int Trajectory::GetPDGEncoding() const
{
  return _pdef->GetPDGEncoding();
}



G4ThreeVector Trajectory::GetInitialMomentum() const
{

}



void Trajectory::AppendStep(const G4Step* aStep)
{
  // _trjpoints->push_back( new G4TrajectoryPoint(aStep->GetPostStepPoint()->GetPosition() ));
}



void Trajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  // if(!secondTrajectory) return;
 
  // G4Trajectory* seco = (G4Trajectory*)secondTrajectory;
  // G4int ent = seco->GetPointEntries();

  // // initial point of the second trajectory should not be merged
  // for(G4int i=1;i<ent;i++) { 
  //   positionRecord->push_back((*(seco->positionRecord))[i]);
  //   //    positionRecord->push_back(seco->positionRecord->removeAt(1));
  // }

  // delete (*seco->positionRecord)[0];
  // seco->positionRecord->clear();
}



// void Trajectory::ShowTrajectory(std::ostream& os) const
// {
//   // Invoke the default implementation in the base class
//   G4VTrajectory::ShowTrajectory(os);
// }



// void Trajectory::DrawTrajectory(G4int i_mode) const
// {
//   // Invoke the default implementation in the base class
//   G4VTrajectory::DrawTrajectory(i_mode);
// }



// const std::map<G4String,G4AttDef>* Trajectory::GetAttDefs() const
// {
//   G4bool isNew;
//   std::map<G4String,G4AttDef>* store
//     = G4AttDefStore::GetInstance("Trajectory",isNew);
  
//   if (isNew) {
//     G4String ID("ID");
//     (*store)[ID] = G4AttDef(ID,"Track ID","Physics","","G4int");

//     G4String PID("PID");
//     (*store)[PID] = G4AttDef(PID,"Parent ID","Physics","","G4int");

//     G4String PN("PN");
//     (*store)[PN] = G4AttDef(PN,"Particle Name","Physics","","G4String");

//     G4String Ch("Ch");
//     (*store)[Ch] = G4AttDef(Ch,"Charge","Physics","e+","G4double");

//     G4String PDG("PDG");
//     (*store)[PDG] = G4AttDef(PDG,"PDG Encoding","Physics","","G4int");

//     G4String IKE("IKE");
//     (*store)[IKE] = G4AttDef(IKE, "Initial kinetic energy",
//       "Physics","G4BestUnit","G4double");

//     G4String IMom("IMom");
//     (*store)[IMom] = G4AttDef(IMom, "Initial momentum",
//       "Physics","G4BestUnit","G4ThreeVector");

//     G4String IMag("IMag");
//     (*store)[IMag] = G4AttDef(IMag, "Initial momentum magnitude",
//       "Physics","G4BestUnit","G4double");

//     G4String NTP("NTP");
//     (*store)[NTP] = G4AttDef(NTP,"No. of points","Physics","","G4int");
//   }

//   return store;
// }



// std::vector<G4AttValue>* Trajectory::CreateAttValues() const
// {
//   std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
 
//   values->push_back
//     (G4AttValue("ID",G4UIcommand::ConvertToString(fTrackID),""));
 
//   values->push_back
//     (G4AttValue("PID",G4UIcommand::ConvertToString(fParentID),""));
 
//   values->push_back
//     (G4AttValue("PN",ParticleName,""));
 
//   values->push_back
//     (G4AttValue("Ch",G4UIcommand::ConvertToString(PDGCharge),""));
 
//   values->push_back
//     (G4AttValue("PDG",G4UIcommand::ConvertToString(PDGEncoding),""));
 
//   values->push_back
//     (G4AttValue("IKE",G4BestUnit(initialKineticEnergy,"Energy"),""));
 
//   values->push_back
//     (G4AttValue("IMom",G4BestUnit(initialMomentum,"Energy"),""));
 
//   values->push_back
//     (G4AttValue("IMag",G4BestUnit(initialMomentum.mag(),"Energy"),""));
 
//   values->push_back
//     (G4AttValue("NTP",G4UIcommand::ConvertToString(GetPointEntries()),""));
 
//   return values;
// }

 