// ----------------------------------------------------------------------------
///  \file   PmtHit.h
///  \brief  
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     10 Feb 2010  
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __PMT_HIT__
#define __PMT_HIT__

#include "PmtHit.h"
#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>

namespace bhep { class hit; }


namespace nexus {

  ///

  class PmtHit: public G4VHit
  {
  public:
    G4int GetPmtID();
    void  SetPmtID(G4int id);

    void Fill(G4double);

    void SetBinSize(G4double b);
    G4double GetBinSize();

    std::vector<G4double> GetTimes() const;
    std::vector<G4int> GetCounts() const;

    bhep::hit* ToBhep() const;
    
  public:
    /// constructor
    PmtHit();
    /// copy-constructor
    PmtHit(const PmtHit&);
    /// destructor
    ~PmtHit();
    
    /// assignement operator
    const PmtHit& operator =(const PmtHit&);
    /// equality operator
    G4int operator ==(const PmtHit&) const;

    /// memory allocation
    void* operator new(size_t);
    /// memory deallocation
    void operator delete(void*);
  
  private:

    static G4double GetKey(std::pair<G4double,G4int>);
    static G4int GetValue(std::pair<G4double,G4int>);
  
  private:
    G4int _pmt_id;
    G4double _bin_size;

    std::map<G4double, G4int> _histogram;
  };


  // inline definitions ....................................
  
  typedef G4THitsCollection<PmtHit> PmtHitsCollection;

  extern G4Allocator<PmtHit> PmtHitAllocator;

  inline void* PmtHit::operator new(size_t) 
  { void* hit;
    hit = (void*) PmtHitAllocator.MallocSingle();
    return hit; }

  inline void PmtHit::operator delete(void* hit) 
  { PmtHitAllocator.FreeSingle((PmtHit*) hit); }

  inline G4int PmtHit::GetPmtID() { return _pmt_id; }

  inline void PmtHit::SetPmtID(G4int id) { _pmt_id = id; }
  
  inline G4double PmtHit::GetBinSize() { return _bin_size; }

  inline void PmtHit::SetBinSize(G4double bs) { _bin_size = bs; }

  inline G4double PmtHit::GetKey(std::pair<G4double,G4int> p)
  { return p.first; }

  inline G4int PmtHit::GetValue(std::pair<G4double,G4int> p)
  { return p.second; }


} // end namespace nexus

#endif
