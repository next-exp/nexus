// ----------------------------------------------------------------------------
// nexus | PmtHit.cc
//
// This class describes the charge detected by a photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PmtHit.h"


using namespace nexus;


G4Allocator<PmtHit> PmtHitAllocator;



PmtHit::PmtHit():
  G4VHit(), pmt_id_(-1.), bin_size_(0.)
{
}



PmtHit::PmtHit(G4int id, const G4ThreeVector& position, G4double bin_size):
  G4VHit(), pmt_id_(id),  bin_size_(bin_size), position_(position)
{
}



PmtHit::~PmtHit()
{
}



PmtHit::PmtHit(const PmtHit& other): G4VHit()
{
  *this = other;
}



const PmtHit& PmtHit::operator=(const PmtHit& other)
{
  pmt_id_    = other.pmt_id_;
  bin_size_  = other.bin_size_;
  position_  = other.position_;
  histogram_ = other.histogram_;

  return *this;
}



G4int PmtHit::operator==(const PmtHit& other) const
{
  return (this==&other) ? 1 : 0;
}



void PmtHit::SetBinSize(G4double bin_size)
{
  if (histogram_.size() == 0) {
    bin_size_ = bin_size;
  }
  else {
    G4String msg = "A PmtHit cannot be rebinned once it has been filled.";
    G4Exception("[PmtHit]", "SetBinSize()", JustWarning, msg);
  }
}



void PmtHit::Fill(G4double time, G4int counts)
{
  G4double time_bin = floor(time/bin_size_) * bin_size_;
  histogram_[time_bin] += counts;
}

void PmtHit::FillWavelengths(G4double time, G4double wvl)
{
  wavelengths_[time] = wvl;
}
