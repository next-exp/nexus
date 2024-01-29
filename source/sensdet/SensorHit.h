// ----------------------------------------------------------------------------
// nexus | SensorHit.h
//
// This class describes the charge detected by a photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PMT_HIT_H
#define PMT_HIT_H

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>


namespace nexus {

  class SensorHit: public G4VHit
  {
  public:
    /// Default constructor
    SensorHit();
    /// Constructor providing the detector ID and position
    SensorHit(G4int ID, const G4ThreeVector& position, G4double bin_size);
    /// Copy-constructor
    SensorHit(const SensorHit&);
    /// Destructor
    ~SensorHit();

    /// Assignement operator
    const SensorHit& operator=(const SensorHit&);
    /// Equality operator
    G4int operator==(const SensorHit&) const;

    /// Memory allocation
    void* operator new(size_t);
    /// Memory deallocation
     void operator delete(void*);

    /// Returns the detector ID code
    G4int GetSensorID() const;
    /// Sets the detector ID code
    void  SetSensorID(G4int);

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

    const std::map<G4double, G4int>& GetHistogram() const;

  private:
    G4int sns_id_;           ///< Detector ID number
    G4double bin_size_;      ///< Size of time bin
    G4ThreeVector position_; ///< Detector position

    /// Sparse histogram with number of photons detected per time bin
    std::map<G4double, G4int> histogram_;
  };

} // namespace nexus


typedef G4THitsCollection<nexus::SensorHit> SensorHitsCollection;
extern G4Allocator<nexus::SensorHit> SensorHitAllocator;


// INLINE DEFINITIONS ////////////////////////////////////////////////

namespace nexus {

  inline void* SensorHit::operator new(size_t)
  { return ((void*) SensorHitAllocator.MallocSingle()); }

  inline void SensorHit::operator delete(void* hit)
  { SensorHitAllocator.FreeSingle((SensorHit*) hit); }

  inline G4int SensorHit::GetSensorID() const { return sns_id_; }
  inline void SensorHit::SetSensorID(G4int id) { sns_id_ = id; }

  inline G4double SensorHit::GetBinSize() const { return bin_size_; }

  inline G4ThreeVector SensorHit::GetPosition() const { return position_; }
  inline void SensorHit::SetPosition(const G4ThreeVector& p) { position_ = p; }

  inline const std::map<G4double, G4int>& SensorHit::GetHistogram() const
  { return histogram_; }

} // namespace nexus

#endif
