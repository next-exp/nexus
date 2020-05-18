// ----------------------------------------------------------------------------
// nexus | AddUserInfoToPV.h
//
// This class is a utility to add the information about muon directions
// to the primary vertex of the MuonGenerator class.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __AddUser_InfoToPV__
#define __AddUser_InfoToPV__

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
    G4double GetTheta();
    G4double GetPhi();

  private:

    G4double theta_;
    G4double phi_;
  };

  inline G4double AddUserInfoToPV::GetTheta()
  { return theta_; }
  inline G4double AddUserInfoToPV::GetPhi()
  { return phi_; }

} // end namespace nexus

#endif
