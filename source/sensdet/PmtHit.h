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
#include <G4ThreeVector.hh>

namespace bhep { class hit; }


namespace nexus {

  ///

  class PmtHit: public G4VHit
  {
  public:
    /// Default constructor
    PmtHit(G4double bin_size=0.1*microsecond);
    /// Constructor providing the detector ID and position
    PmtHit(G4int ID, const G4ThreeVector& position, G4double bin_size);
    /// Copy-constructor
    PmtHit(const PmtHit&);
    /// Destructor
    ~PmtHit();
    
    /// Assignement operator
    const PmtHit& operator=(const PmtHit&);
    /// Equality operator
    G4int operator==(const PmtHit&) const;
    
    /// Memory allocation
    void* operator new(size_t);
    /// Memory deallocation
    void operator delete(void*);
    
    /// Returns the detector ID code
    G4int GetPmtID() const;
    /// Sets the detector ID code
    void  SetPmtID(G4int);

    /// Returns the position of the detector
    G4ThreeVector GetPosition() const;
    /// Sets the position of the detector
    void SetPosition(const G4ThreeVector&);

    /// Returns the bin size of the histogram
    G4double GetBinSize() const;
    /// Sets the bin size of the histogram. This can only be done
    /// while the histogram is empty (rebinning is not supported).
    void SetBinSize(G4double);

    /// Adds counts to a given time bin
    void Fill(G4double time, G4int counts=1);
    
    std::vector<G4double> GetTimes() const;
    std::vector<G4int> GetCounts() const;

    /// Returns a copy of the PmtHit in bhep format. The caller owns 
    /// the resulting object, thus having deletion responsability.
    bhep::hit* ToBhep() const;
    
  private:

    static G4double GetKey(std::pair<G4double,G4int>);
    static G4int GetValue(std::pair<G4double,G4int>);
  
  private:

    G4int _pmt_id; ///< Detector ID number
    G4ThreeVector _position; ///< Detector position

    G4double _bin_size; ///< Size of time bin

    /// Sparse histogram with the no. of photons detected per time bin
    std::map<G4double, G4int> _histogram; 
  };


  // inline definitions ..............................................
  
  typedef G4THitsCollection<PmtHit> PmtHitsCollection;

  extern G4Allocator<PmtHit> PmtHitAllocator;

  inline void* PmtHit::operator new(size_t) 
  { void* hit; hit = (void*) PmtHitAllocator.MallocSingle();
    return hit; }

  inline void PmtHit::operator delete(void* hit) 
  { PmtHitAllocator.FreeSingle((PmtHit*) hit); }

  inline G4int PmtHit::GetPmtID() const { return _pmt_id; }

  inline void PmtHit::SetPmtID(G4int id) { _pmt_id = id; }
  
  inline G4double PmtHit::GetBinSize() const { return _bin_size; }

  inline G4ThreeVector PmtHit::GetPosition() const { return _position; }

  inline void PmtHit::SetPosition(const G4ThreeVector& p) { _position = p; }

  inline G4double PmtHit::GetKey(std::pair<G4double,G4int> p)
  { return p.first; }

  inline G4int PmtHit::GetValue(std::pair<G4double,G4int> p)
  { return p.second; }


} // end namespace nexus

#endif
