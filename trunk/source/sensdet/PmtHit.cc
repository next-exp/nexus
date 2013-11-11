// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>
//  Created: 15 Feb 2010
//
//  Copyother (c) 2010-2013 NEXT Collaboration. All others reserved.
// ----------------------------------------------------------------------------

#include "PmtHit.h"


using namespace nexus;


G4Allocator<PmtHit> PmtHitAllocator;
  
  
  
PmtHit::PmtHit(): 
  G4VHit(), _pmt_id(-1.), _bin_size(0.)
{
}
  
  

PmtHit::PmtHit(G4int id, const G4ThreeVector& position, G4double bin_size):
  G4VHit(), _pmt_id(id), _position(position), _bin_size(bin_size)
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
  _pmt_id    = other._pmt_id;
  _bin_size  = other._bin_size;
  _position  = other._position;
  _histogram = other._histogram; 

  return *this;
}



G4int PmtHit::operator==(const PmtHit& other) const
{
  return (this==&other) ? 1 : 0;
}
  
  
  
void PmtHit::SetBinSize(G4double bin_size)
{
  if (_histogram.size() == 0) {
    _bin_size = bin_size;
  }
  else {
    G4String msg = "A PmtHit cannot be rebinned once it has been filled.";
    G4Exception("[PmtHit]", "SetBinSize()", JustWarning, msg);
  }
}
  
  
  
void PmtHit::Fill(G4double time, G4int counts)
{
  G4double time_bin = floor(time/_bin_size) * _bin_size;
  _histogram[time_bin] += counts;
}
  
