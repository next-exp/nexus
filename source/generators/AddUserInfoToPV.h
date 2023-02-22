// ----------------------------------------------------------------------------
// nexus | AddUserInfoToPV.h
//
// This class is a utility to add the information about muon directions
// to the primary vertex of the MuonGenerator class.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef ADD_USERINFO_TO_PV_H
#define ADD_USERINFO_TO_PV_H

#include <G4VUserPrimaryVertexInformation.hh>
#include "globals.hh"

namespace nexus {

  class AddUserInfoToPV: public G4VUserPrimaryVertexInformation
  {
  public:
    //constructor
    AddUserInfoToPV(G4double theta,G4double phi);
    //destructor
    ~AddUserInfoToPV();

    void Print() const;
    G4double GetZenith();
    G4double GetAzimuth();

  private:

    G4double zenith_;
    G4double azimuth_;
  };

  inline G4double AddUserInfoToPV::GetZenith()
  { return zenith_; }
  inline G4double AddUserInfoToPV::GetAzimuth()
  { return azimuth_; }

} // end namespace nexus

#endif
