// -----------------------------------------------------------------------------
///  \file   RadiusDependentDriftField.h
///  \brief  
///  
///  \author  <justo.martin-albo@ific.uv.es>
///  \date    4 September 2013
///  \version $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// -----------------------------------------------------------------------------

#ifndef RADIUS_DEPENDENT_DRIFT_FIELD_H
#define RADIUS_DEPENDENT_DRIFT_FIELD_H

#include "BaseDriftField.h"
#include <G4LorentzVector.hh>


namespace nexus {
  
  class RadiusDependentDriftField: public BaseDriftField
  {
  public:
    /// Constructor
    RadiusDependentDriftField();
    /// Destructor
    ~RadiusDependentDriftField();
  
    virtual G4double Drift(G4LorentzVector&);

    virtual G4LorentzVector GeneratePointAlongDriftLine(const G4LorentzVector&, const G4LorentzVector&);

    //virtual G4double LightYield() const;
  };

} // end namespace nexus

#endif
 

