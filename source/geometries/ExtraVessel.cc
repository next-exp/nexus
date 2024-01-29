// ----------------------------------------------------------------------------
// nexus | ExtraVessel.cc
//
// Volume used to simulate radioactive background from materials such as
// connectors, feedthroughs and adapter boards, placed outside the vessel,
// behind the tracking plane.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ExtraVessel.h"
#include "MaterialsList.h"
#include "CylinderPointSamplerLegacy.h"
#include "Visibilities.h"

#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>


namespace nexus {

  using namespace CLHEP;

  ExtraVessel::ExtraVessel():
    GeometryBase(),
    // Dimensions
    diameter_ (10. * cm),
    thickness_ (1. * mm),
    visibility_(1)
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/ExtraVessel/",
                                  "Control commands of ExtraVessel geometry.");
    msg_->DeclareProperty("visibility", visibility_,
                          "visibility of ExtraVessel geometry");
  }


   ExtraVessel::~ExtraVessel()
  {
    delete generic_gen_;
  }


  void ExtraVessel::Construct()
  {
    // GENERIC VOLUME //////////////////////////////////////////////////////

    G4Tubs* generic_solid =
      new G4Tubs("EXTRA_VESSEL", 0., diameter_/2., thickness_/2.,
        0., twopi);

    // We model the material with a generic material of the density of FR4.
    // Feedthrough, adapter boards and connectors are made of FR4.
    // The O-rings are made of metal (some of them) and nitrile or viton.

    G4Material* mat = materials::FR4();

    G4LogicalVolume* generic_logic =
      new G4LogicalVolume(generic_solid, mat, "EXTRA_VESSEL");
    this->SetLogicalVolume(generic_logic);


    // VISIBILITIES //////////////////////////////////////////////////
    if (visibility_) {
      G4VisAttributes generic_col = nexus::Yellow();
      generic_col.SetForceSolid(true);
      generic_logic->SetVisAttributes(generic_col);
    } else {
      generic_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }


    // VERTEX GENERATOR ////////////////////////////////////////////

    generic_gen_ =
      new CylinderPointSamplerLegacy(0, thickness_, diameter_/2., 0.,
                                     G4ThreeVector (0., 0., 0.));
  }


  G4ThreeVector ExtraVessel::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "EXTRA_VESSEL") {
        vertex = generic_gen_->GenerateVertex("BODY_VOL");
    }

    return vertex;
  }

} // end namespace nexus
