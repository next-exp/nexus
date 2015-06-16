//---------------------------------------------------------------------------
//   \file   AddUserInfoToPV
///  \brief  Add User info (muons) to the Primary Vertex
///  
///  \author   Neus Lopez March <neus.lopez@ific.uv.es>    
///  \date     30 Jan 2014
/// 
///
///  Copyright (c) 2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "AddUserInfoToPV.h"

using namespace nexus;

AddUserInfoToPV::AddUserInfoToPV(G4double theta, G4double phi):
  //muon generator angles
  _Theta(theta),_Phi(phi)
{
}

AddUserInfoToPV::~AddUserInfoToPV()
{

}

void AddUserInfoToPV::Print() const
{ 
  //  std::cout<<"in print"<<std::endl;  
}


