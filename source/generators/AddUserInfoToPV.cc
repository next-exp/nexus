// ----------------------------------------------------------------------------
// nexus | AddUserInfoToPV.cc
//
// This class is a utility to add the information about muon directions
// to the primary vertex of the MuonGenerator class.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "AddUserInfoToPV.h"

using namespace nexus;

AddUserInfoToPV::AddUserInfoToPV(G4double theta, G4double phi):
  theta_(theta),phi_(phi)
{
}

AddUserInfoToPV::~AddUserInfoToPV()
{

}

void AddUserInfoToPV::Print() const
{
  //  std::cout<<"in print"<<std::endl;
}
