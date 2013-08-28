// ----------------------------------------------------------------------------
///  \file   DriftTrackInfo.h
///  \brief  Store of drift-related quantities.
///  
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     10 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __DRIFT_TRACK_INFO__
#define __DRIFT_TRACK_INFO__

#include <G4VUserTrackInformation.hh>


namespace nexus {

  /// This class object stores quantities related to the drift of
  /// ionization particles (ionization clusters or ionization electrons).
  /// It inherits from G4VUserTrackInformation so that it can be attached
  /// to tracks.
  
  class DriftTrackInfo: public G4VUserTrackInformation
  {
  public:
    /// Constructor
    DriftTrackInfo();
    
    /// Destructor
    ~DriftTrackInfo();

    
    G4bool GetDriftStatus() const;
    void SetDriftStatus(G4bool);

    G4int GetNumberOfCharges() const;
    void SetNumberOfCharges(G4int);

    G4double GetELLightYield() const;
    void SetELLightYield(G4double);
    
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
    G4bool _status;
    G4int _num_charges; ///< No. of charges in an ionization cluster
    G4int _el_yield;
    G4double _drift_length; ///< Accumulated drift length
    G4double _drift_time; 
    G4double _transv_spread;
    G4double _longit_spread;
    G4double _time_spread;
  };
  
  
  // inline methods ..................................................

  inline DriftTrackInfo::DriftTrackInfo(): 
    _status(false), _num_charges(0), _drift_length(0.), _drift_time(0.), 
    _longit_spread(0.), _transv_spread(0.), _time_spread(0.) {}
  
  inline DriftTrackInfo::~DriftTrackInfo() {}

  inline void DriftTrackInfo::SetDriftStatus(G4bool s) 
  { _status = s; }

  inline G4bool DriftTrackInfo::GetDriftStatus() const 
  { return _status; }

  inline void DriftTrackInfo::SetNumberOfCharges(G4int nc) 
  { _num_charges = nc; }

  inline G4int DriftTrackInfo::GetNumberOfCharges() const
  { return _num_charges; }

  inline void DriftTrackInfo::SetELLightYield(G4double y)
  { _el_yield = y;
  }

  inline G4double DriftTrackInfo::GetELLightYield() const
  { return _el_yield; }

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
