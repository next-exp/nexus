// ----------------------------------------------------------------------------
// nexus | RadiusDependentDriftField.cc
//
// Drift field varying with radial coordinate.
//
// ****************************** WARNING *******************************
// ***************** This class has not been implemented ****************
// **********************************************************************
//
// The NEXT Collaboration
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
