// ----------------------------------------------------------------------------
///  \file   DriftTrackInfo.h
///  \brief  Store of drift-related quantities
///  
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     10 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __DRIFT_TRACK_INFO__
#define __DRIFT_TRACK_INFO__

#include <G4VUserTrackInformation.hh>
#include <G4LorentzVector.hh>

namespace nexus {

  enum DriftStatus { ALIVE, ATTACHED };


  /// This class object stores quantities related to the drift of
  /// ionization electrons. It inherits from G4VUserTrackInformation 
  /// so that it can be attached to tracks.
  
  class DriftTrackInfo: public G4VUserTrackInformation
  {
  public:
    /// Constructor
    DriftTrackInfo();
    
    /// Destructor
    ~DriftTrackInfo();

    // setters, getters

    G4LorentzVector GetPreDriftPoint() const;
    void SetPreDriftPoint(G4LorentzVector);

    G4LorentzVector GetPostDriftPoint() const;
    void SetPostDriftPoint(G4LorentzVector);
    
    DriftStatus GetDriftStatus() const;
    void SetDriftStatus(DriftStatus);
    
    G4int GetNumberOfPhotons() const;
    void SetNumberOfPhotons(G4int);

    G4double GetLongitudinalSpread() const;
    void SetLongitudinalSpread(double);

    G4double GetTimeSpread() const;
    void SetTimeSpread(double);

    G4double GetTransverseSpread() const;
    void SetTransverseSpread(double);

    G4double GetDriftTime() const;
    void SetDriftTime(G4double);

    G4double GetDriftLength() const;
    void SetDriftLength(G4double);

  private:
    void Print() const;

  private:
    DriftStatus _status; ///< Status of the drifting track
    G4LorentzVector _prepoint, _postpoint;
    G4double _drift_time;
    G4double _drift_length;
    G4double _transv_spread;
    G4double _longit_spread;
    G4double _time_spread;
    G4int _num_ph; ///< Number of photons generated during the drift
  };
  
  
  // INLINE METHODS //////////////////////////////////////////////////

  inline DriftTrackInfo::DriftTrackInfo(): 
    _prepoint(), _postpoint(), _status(ALIVE), _drift_length(0.), 
    _drift_time(0.), _longit_spread(0.), _transv_spread(0.), 
    _time_spread(0.) {}
  
  inline DriftTrackInfo::~DriftTrackInfo() {}

  inline void DriftTrackInfo::SetPreDriftPoint(G4LorentzVector p)
  { _prepoint = p; }
  inline G4LorentzVector DriftTrackInfo::GetPreDriftPoint() const
  { return _prepoint; }

  inline void DriftTrackInfo::SetPostDriftPoint(G4LorentzVector p)
  { _postpoint = p; }
  inline G4LorentzVector DriftTrackInfo::GetPostDriftPoint() const
  { return _postpoint; }

  inline void DriftTrackInfo::SetDriftStatus(DriftStatus s) 
  { _status = s; }
  inline DriftStatus DriftTrackInfo::GetDriftStatus() const 
  { return _status; }

  inline void DriftTrackInfo::SetNumberOfPhotons(G4int nph)
  { _num_ph = nph; }
  inline G4int DriftTrackInfo::GetNumberOfPhotons() const
  { return _num_ph; }

  inline void DriftTrackInfo::SetTimeSpread(G4double ts) 
  { _time_spread = ts; }
  inline G4double DriftTrackInfo::GetTimeSpread() const 
  { return _time_spread; }

  inline void DriftTrackInfo::SetLongitudinalSpread(double ls) 
  { _longit_spread = ls; }
  inline G4double DriftTrackInfo::GetLongitudinalSpread() const
  { return _longit_spread; }

  inline void DriftTrackInfo::SetTransverseSpread(double ts)
  { _transv_spread = ts; }
  inline G4double DriftTrackInfo::GetTransverseSpread() const
  { return _transv_spread; }

  inline void DriftTrackInfo::SetDriftLength(G4double dl)
  { _drift_length = dl; }
  inline G4double DriftTrackInfo::GetDriftLength() const
  { return _drift_length; }

  inline void DriftTrackInfo::SetDriftTime(G4double dt)
  { _drift_time = dt; }
  inline G4double DriftTrackInfo::GetDriftTime() const
  { return _drift_time; }

  inline void DriftTrackInfo::Print() const {}

} // end namespace nexus

#endif
