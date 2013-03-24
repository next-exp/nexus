// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J. Martin-Albo <jmalbos@ific.uv.es>
//  Created: 15 Feb 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PmtHit.h"

#include <iterator>

using namespace nexus;


G4Allocator<PmtHit> PmtHitAllocator;
  
  
  
PmtHit::PmtHit(G4double bin_size): 
  G4VHit(), _pmt_id(0), _bin_size(bin_size)
{
}
  
  
  
PmtHit::PmtHit(G4int ID, const G4ThreeVector& position, G4double bin_size): 
  G4VHit(), _pmt_id(ID), _position(position)
{
}
  
  
  
PmtHit::~PmtHit()
{
}
  
  
  
PmtHit::PmtHit(const PmtHit& right): G4VHit()
{  
  _pmt_id    = right._pmt_id;
  _bin_size  = right._bin_size;
  _position  = right._position;
  _histogram = right._histogram; 
}


  
const PmtHit& PmtHit::operator=(const PmtHit& right)
{
  _pmt_id    = right._pmt_id;
  _bin_size  = right._bin_size;
  _position  = right._position;
  _histogram = right._histogram; 

  return *this;
}
  
  
  
void PmtHit::SetBinSize(G4double bin_size)
{
  if (_histogram.size() == 0) {
    _bin_size = bin_size;
  }
  else {
    G4cout << "[PmtHit] WARNING: PmtHits cannot be rebinned "
	   << "(you tried to modify the bin size of a non-empty PmtHit)."
	   << G4endl;
  }
}
  
  
  
void PmtHit::Fill(G4double time, G4int counts)
{
  G4double time_bin = floor(time/_bin_size) * _bin_size;
  _histogram[time_bin] += counts;
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



// bhep::hit* PmtHit::ToBhep() const
// {
//   bhep::hit* bhit = new bhep::hit(bhep::TRUTH, "PmtHit");
    
//   std::map<G4double, G4int>::const_iterator it;

//   G4int total_counts = 0;

//   for (it=_histogram.begin(); it!=_histogram.end(); ++it) {

//     G4int bin_counts = (*it).second;
//     total_counts += bin_counts;

//     bhit->set_amplitude(bin_counts, (*it).first);
//     bhit->set_point(bhep::Point3D(_position.x(),
// 				  _position.y(),
// 				  _position.z()));

//     //      G4cout<<"position = "<<_position.x()<<" "<<_position.y()<<" "<<_position.z()<<G4endl;

//   }
    
//   bhit->set_amplitude(total_counts);
    
//   int ID = this->GetPmtID();

//   bhit->add_property("ID", ID);
//   return bhit;
// }

  

