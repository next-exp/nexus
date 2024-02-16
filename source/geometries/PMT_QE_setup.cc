// ----------------------------------------------------------------------------
// nexus | PMT_QE_setup.cc
//
// Geometry of a set-up to extract the quantum efficiency of a photomultiplier.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PMT_QE_setup.h"
#include "MaterialsList.h"
#include "SiPM11.h"
#include "OpticalMaterialProperties.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <Randomize.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  PMT_QE_setup::PMT_QE_setup(): GeometryBase()
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/PMT_QE_setup/",
				"Control commands of geometry PMT_QE_setup.");

    G4GenericMessenger::Command& dist_cmd =
      msg_->DeclareProperty("z_dist", z_dist_,
			    "Distance of the generation point in z from the surface of the PMT window.");
    dist_cmd.SetUnitCategory("Length");
    dist_cmd.SetParameterName("z_dist", false);
    dist_cmd.SetRange("z_dist>0.");
  }



  PMT_QE_setup::~PMT_QE_setup()
  {
  }



  void PMT_QE_setup::Construct()
  {
    // CHAMBER ///////////////////////////////////////////////////////

    // The parameterized width, height and length are internal dimensions.
    // The chamber thickness is added to obtain the external (total) size.
    G4double width = 50.*cm;
    G4double height = 50.*cm;
    length_ = 50.*cm;
    G4double thickn = 1.*cm;
    G4double X = width  + 2.*thickn;
    G4double Y = height + 2.*thickn;
    G4double Z = length_ + 2.*thickn;

    G4Box* chamber_solid = new G4Box("CHAMBER", X/2., Y/2., Z/2.);

    G4Material* copper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

    G4LogicalVolume* chamber_logic =
      new G4LogicalVolume(chamber_solid, copper, "CHAMBER");
    this->SetLogicalVolume(chamber_logic);

    // GAS ///////////////////////////////////////////////////////////
    G4double gxe_pressure = 10*bar;

    G4Box* gas_solid = new G4Box("GAS", width/2., height/2., length_/2.);

    G4Material* gxe = materials::GXe(gxe_pressure);
    gxe->SetMaterialPropertiesTable(opticalprops::GXe(gxe_pressure));
    // G4Material* air =
    //   G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

    G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");

    new G4PVPlacement(0, G4ThreeVector(0,0,0), gas_logic, "GAS",
		      chamber_logic, false, 0, true);

    // Positioning of the PMT /////////////////////////////////////////
    pmt_.Construct();
    G4LogicalVolume* pmt_logic = pmt_.GetLogicalVolume();
    //   pmt_length_ = pmt_.Length() // this is R7378A
    pmt_length_ = 20*cm; // this is R11410

    new G4PVPlacement(0, G4ThreeVector(0.,0.,-length_/2.+pmt_length_/2.),
		      pmt_logic, "PMT",
		      gas_logic, false, 0, true);

    // Positioning of the teflon panel
    // G4Box* teflon_solid = new G4Box("GAS", 22.*cm/2., 50.*cm/2., thickn/2.);
    // G4Material* ptfe = materials::PEEK();
    // G4LogicalVolume* teflon_logic = new G4LogicalVolume(teflon_solid, ptfe, "TEFLON");

    // G4RotationMatrix* rotdb = new G4RotationMatrix();
    // rotdb->rotateY(-pi/2.);
    // new G4PVPlacement(0, G4ThreeVector(0.*cm, 0. ,-length_/2.+_pmtlength_ + z_dist_ + 1.*cm),
    // 		      teflon_logic, "TEFLON", gas_logic, false, 0, true);
    // G4RotationMatrix* rotdb2 = new G4RotationMatrix();
    // rotdb2->rotateY(pi/2.);
    // new G4PVPlacement(rotdb2, G4ThreeVector(-10.*cm, 0., -length_/2.+_pmtlength_ + z_dist_ + 1.*cm),
    //  		      teflon_logic, "TEFLON", gas_logic, false, 2, true);

    // Optical surface
    // G4OpticalSurface* teflon_opsur = new G4OpticalSurface("TEFLON_OPSURF");
    // teflon_opsur->SetType(dielectric_metal);
    // teflon_opsur->SetModel(unified);
    // teflon_opsur->SetFinish(ground);
    // teflon_opsur->SetSigmaAlpha(0.0000001); // it does not affect, because the reflection is totally lambertian.
    // teflon_opsur->SetMaterialPropertiesTable(opticalprops::PTFE());

    // new G4LogicalSkinSurface("TEFLON_OPSURF", teflon_logic, teflon_opsur);

  }

  G4ThreeVector PMT_QE_setup::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector point;

    if (region == "CIRCLE"){
      //     G4double pmt_diameter = pmt_.Diameter(); // this is R7378A
      G4double pmt_diameter = 64.*mm;  // this is R11410
      G4double r_max = pmt_diameter/2.;
      G4double r = G4UniformRand()*r_max;
      G4double phi = G4UniformRand()*2*pi;
      G4double x = r*cos(phi);
      G4double y = r*sin(phi);

      point =  G4ThreeVector(x, y,-length_/2.+pmt_length_+z_dist_);

    } else if (region == "POINT") {
      point = G4ThreeVector(0, 0, -length_/2.+pmt_length_+z_dist_);
    }

    return point;
  }


} //end namespace nexus
