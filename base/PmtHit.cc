// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J. Martin-Albo <jmalbos@ific.uv.es>
//  Created: 15 Feb 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PmtHit.h"

#include <bhep/hit.h>

#include <iterator>

namespace nexus {


  G4Allocator<PmtHit> PmtHitAllocator;
  
  

  PmtHit::PmtHit(): G4VHit(), _pmt_id(0), _bin_size(1.*ns)
  {
  }

  
  
  PmtHit::~PmtHit()
  {
  }
  
  
  
  PmtHit::PmtHit(const PmtHit& right): G4VHit()
  {
    *this = right;
  }


  
  const PmtHit& PmtHit::operator =(const PmtHit& right)
  {
    _pmt_id   = right._pmt_id;
    _bin_size = right._bin_size;

    return *this;
  }



  void PmtHit::Fill(G4double time)
  {
    G4double time_bin = floor(time/_bin_size) * _bin_size;
    _histogram[time_bin] += 1;
  }
  
  
  
  std::vector<G4double> PmtHit::GetTimes() const
  {
    std::vector<G4double> times;
    times.resize(_histogram.size());
    std::transform(_histogram.begin(),_histogram.end(),times.begin(),GetKey);

    return times;
  }
  
  
  
  std::vector<G4int> PmtHit::GetCounts() const
  {
    std::vector<G4int> counts;
    counts.resize(_histogram.size());
    std::transform(_histogram.begin(),_histogram.end(),counts.begin(),GetValue);

    return counts;
  }



  bhep::hit* PmtHit::ToBhep() const
  {
    bhep::hit* bhit = new bhep::hit(bhep::TRUTH, "pmt");
    
    std::map<G4double, G4int>::const_iterator it;

    for (it=_histogram.begin(); it!=_histogram.end(); ++it) {
      bhit->set_amplitude((*it).second, (*it).first);
    }
    
    return bhit;
  }

  
} // end namespace nexus
