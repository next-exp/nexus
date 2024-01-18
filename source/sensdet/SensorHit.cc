// ----------------------------------------------------------------------------
// nexus | SensorHit.cc
//
// This class describes the charge detected by a photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SensorHit.h"


using namespace nexus;


G4Allocator<SensorHit> SensorHitAllocator;



SensorHit::SensorHit():
  G4VHit(), sns_id_(-1.), bin_size_(0.)
{
}



SensorHit::SensorHit(G4int id, const G4ThreeVector& position, G4double bin_size):
  G4VHit(), sns_id_(id),  bin_size_(bin_size), position_(position)
{
}



SensorHit::~SensorHit()
{
}



SensorHit::SensorHit(const SensorHit& other): G4VHit()
{
  *this = other;
}



const SensorHit& SensorHit::operator=(const SensorHit& other)
{
  sns_id_    = other.sns_id_;
  bin_size_  = other.bin_size_;
  position_  = other.position_;
  histogram_ = other.histogram_;

  return *this;
}



G4int SensorHit::operator==(const SensorHit& other) const
{
  return (this==&other) ? 1 : 0;
}



void SensorHit::SetBinSize(G4double bin_size)
{
  if (histogram_.size() == 0) {
    bin_size_ = bin_size;
  }
  else {
    G4String msg = "A SensorHit cannot be rebinned once it has been filled.";
    G4Exception("[SensorHit]", "SetBinSize()", JustWarning, msg);
  }
}



void SensorHit::Fill(G4double time, G4int counts)
{
  G4double time_bin = floor(time/bin_size_) * bin_size_;
  histogram_[time_bin] += counts;
}
