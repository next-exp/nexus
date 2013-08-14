///

#include <next/stacking.hh>

#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4TouchableHistory.hh"
#include "G4TransportationManager.hh"


using namespace bhep;


stacking::stacking(G4String ini_part)
{ 
  //    ini_part_ = ini_part;
  //navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
}

stacking::~stacking()
{
}

G4ClassificationOfNewTrack stacking::ClassifyNewTrack(const G4Track * aTrack)
{
}


void stacking::NewStage()
{
}
    
void stacking::PrepareNewEvent()
{ 
}
