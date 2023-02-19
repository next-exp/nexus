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

AddUserInfoToPV::AddUserInfoToPV(G4double zenith, G4double azimuth):
  zenith_(zenith),azimuth_(azimuth)
{
}

AddUserInfoToPV::~AddUserInfoToPV()
{

}

void AddUserInfoToPV::Print() const
{
  //  std::cout<<"in print"<<std::endl;
}
