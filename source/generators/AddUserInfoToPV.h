// ----------------------------------------------------------------------------
///  \file   AddUserInfoToPV
///  \brief  Add User info (muons) to the Primary Vertex
///  
///  \author   Neus Lopez March <neus.lopez@ific.uv.es>    
///  \date     30 Jan 2014
/// 
///
///  Copyright (c) 2015 NEXT Collaboration
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
    
    G4double _Theta;
    G4double _Phi;
    //G4VUserPrimaryVertexInformation* userInfo;
  };

  inline G4double AddUserInfoToPV::GetTheta()
  { return _Theta; }
  inline G4double AddUserInfoToPV::GetPhi()
  { return _Phi; }
  
} // end namespace nexus

#endif
  
  
