// -----------------------------------------------------------------------------
// nexus | NextNewRnTube.cc
//
// Tube around the NEXT-WHITE vessel used to shoot backgrounds coming
// from the air, mainly radon in the air attached to the vessel walls.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextNewRnTube.h"
#include "CylinderPointSamplerLegacy.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4UnionSolid.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4NistManager.hh>
#include <G4UserLimits.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextNewRnTube::NextNewRnTube():
    GeometryBase(),
    // Body dimensions
    inner_diam_ (970. *mm),
    length_ (1800. *mm),
    thickness_ (1. *mm)

  {
   /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    msg_->DeclareProperty("RnTube_vis", visibility_, "Radon Tube Visibility");
  }

  void NextNewRnTube::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }

  void NextNewRnTube::Construct()
  {
    ////// RADON TUBE ///////////
    G4Tubs* tube_solid = new G4Tubs("RN_BODY", inner_diam_/2., inner_diam_/2.+thickness_, length_/2.,0.,twopi);
    G4Tubs* endcap_solid =new G4Tubs("RN_ENDCAP", 0., inner_diam_/2.+thickness_, thickness_/2.,0.,twopi);
    G4UnionSolid* rn_solid = new G4UnionSolid("RN_TUBE", tube_solid,endcap_solid,
					      0, G4ThreeVector(0.,0.,-length_/2.-thickness_/2.));
    rn_solid = new G4UnionSolid("RN_TUBE", rn_solid,endcap_solid, 0, G4ThreeVector(0.,0.,length_/2.+thickness_/2.) );
    G4Material* mother_material = mother_logic_->GetMaterial();
    G4LogicalVolume* tube_logic = new G4LogicalVolume(rn_solid,mother_material,"RN_TUBE");
  ////////////////////////////////////////
    ////Limit the uStepMax=Maximum step length, uTrakMax=Maximum total track length,
    //uTimeMax= Maximum global time for a track, uEkinMin= Minimum remaining kinetic energy for a track
    //uRangMin=	 Minimum remaining range for a track
    tube_logic->SetUserLimits(new G4UserLimits( 1E8*m, 1E8*m,1E12 *s,100.*keV,0.));

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), tube_logic, "RN_TUBE", mother_logic_, false, 0,false);

    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes steel_col(G4Colour(.88, .87, .86));
      //steel_col.SetForceSolid(true);
      tube_logic->SetVisAttributes(steel_col);
    }
    else {
      tube_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    // VERTEX GENERATORS   //////////
    tube_gen_ = new CylinderPointSamplerLegacy(inner_diam_/2.,length_,thickness_,thickness_, G4ThreeVector(0.,0.,0.), 0);

    // Calculating some probs
    G4double tube_vol = tube_solid->GetCubicVolume();
    G4double cylinder_vol=   length_*pi*(inner_diam_/2.*inner_diam_/2.);
    std::cout<<"RADON TUBE VOLUME:\t"<<tube_vol<<"\t INNER VOLUME:\t"<<cylinder_vol<<std::endl;
  }

  NextNewRnTube::~NextNewRnTube()
  {
    delete tube_gen_;
  }

  G4ThreeVector NextNewRnTube::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "RN_TUBE") {
      vertex = tube_gen_->GenerateVertex("WHOLE_VOL");
    }
    else {
      G4Exception("[NextNewRnTube]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }
    return vertex;
  }

} //end namespace nexus
