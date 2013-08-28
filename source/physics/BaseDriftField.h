// ----------------------------------------------------------------------------
///  \file   BaseDriftField.h
///  \brief  Abstract base class for description of drift fields.
///  
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     12 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef BASE_DRIFT_FIELD_H
#define BASE_DRIFT_FIELD_H

#include <G4VUserRegionInformation.hh>
#include <G4LorentzVector.hh>

class G4Material;


namespace nexus {

  /// This is an abstract base class for the description of electric 
  /// (or electromagnetic) drift fields. It inherits from 
  /// G4VUserRegionInformation so that it can be passed to the drift
  /// process attached to a G4Region.
  
  class BaseDriftField: public G4VUserRegionInformation
  {
  public:
    /// Constructor
    BaseDriftField();
    /// Destructor
    virtual ~BaseDriftField();

    /// Calculates the final position and time of a charge carrier
    /// drifting under the influence of the field. Returns the step length.
    virtual G4double Drift(G4LorentzVector&) = 0;

    /// Returns a random 4D point (space and time) along a drift line
    virtual G4LorentzVector 
    GeneratePointAlongDriftLine(const G4LorentzVector&) = 0;

    virtual G4double LightYield();

  private:
    void Print() const;
  };

  // Inline definitions /////////////////////////////////////////////
  
  inline BaseDriftField::BaseDriftField() {}
  
  inline BaseDriftField::~BaseDriftField() {}

  inline void BaseDriftField::Print() const {}

  inline G4double BaseDriftField::LightYield() {return 0.;}

} // end namespace nexus

#endif
