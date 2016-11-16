// ----------------------------------------------------------------------------
///  \file   PmtHit.h
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     10 Feb 2010  
///  \version  $Id$
///
///  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __PMT_HIT__
#define __PMT_HIT__

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>


namespace nexus {

  /// TODO. CLASS DESCRIPTION

  class PmtHit: public G4VHit
  {
  public:
    /// Default constructor
    PmtHit();
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
    
    /// Adds arrival time and wavelength of a given detected photons
    void FillWavelengths(G4double time, G4double wvl); 

    const std::map<G4double, G4int>& GetHistogram() const;
    const std::map<G4double, G4double>& GetWavelengths() const;
        
  private:
    G4int _pmt_id;           ///< Detector ID number
    G4double _bin_size;      ///< Size of time bin
    G4ThreeVector _position; ///< Detector position

    /// Sparse histogram with number of photons detected per time bin
    std::map<G4double, G4int> _histogram;

    /// Map to save detected photons wavelengths
    std::map<G4double, G4double> _wavelengths;
  };

} // namespace nexus


typedef G4THitsCollection<nexus::PmtHit> PmtHitsCollection;
extern G4Allocator<nexus::PmtHit> PmtHitAllocator;


// INLINE DEFINITIONS ////////////////////////////////////////////////

namespace nexus {

  inline void* PmtHit::operator new(size_t) 
  { return ((void*) PmtHitAllocator.MallocSingle()); }

  inline void PmtHit::operator delete(void* hit) 
  { PmtHitAllocator.FreeSingle((PmtHit*) hit); }

  inline G4int PmtHit::GetPmtID() const { return _pmt_id; }
  inline void PmtHit::SetPmtID(G4int id) { _pmt_id = id; }
  
  inline G4double PmtHit::GetBinSize() const { return _bin_size; }

  inline G4ThreeVector PmtHit::GetPosition() const { return _position; }
  inline void PmtHit::SetPosition(const G4ThreeVector& p) { _position = p; }

  inline const std::map<G4double, G4int>& PmtHit::GetHistogram() const
  { return _histogram; }

  inline const std::map<G4double, G4double>& PmtHit::GetWavelengths() const
    { return _wavelengths; }

} // namespace nexus

#endif
