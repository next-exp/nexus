// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>
//  Created: $ September 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "RadiusDependentDriftField.h"

using namespace nexus;



RadiusDependentDriftField::RadiusDependentDriftField(): BaseDriftField()
{
}



RadiusDependentDriftField::~RadiusDependentDriftField()
{
}



G4double RadiusDependentDriftField::Drift(G4LorentzVector& /*xyzt*/)
{
  return 0.;
}



G4LorentzVector 
RadiusDependentDriftField::GeneratePointAlongDriftLine(const G4LorentzVector&, const G4LorentzVector&)
{
  return G4LorentzVector();
}
