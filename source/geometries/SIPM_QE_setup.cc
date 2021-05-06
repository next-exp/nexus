// ----------------------------------------------------------------------------
// nexus | SIPM_QE_setup.cc
//
// Geometry of a set-up to extract the quantum efficiency of a SIPM.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SIPM_QE_setup.h"
#include "MaterialsList.h"
#include "BoxPointSampler.h"
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

  SIPM_QE_setup::SIPM_QE_setup(): BaseGeometry()
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/SIPM_QE_setup/",
				"Control commands of geometry SIPM_QE_setup.");

    G4GenericMessenger::Command& dist_cmd =
      msg_->DeclareProperty("z_dist", z_dist_,
			    "Distance of the generation point in z from the surface of the SIPM window.");
    dist_cmd.SetUnitCategory("Length");
    dist_cmd.SetParameterName("z_dist", false);
    dist_cmd.SetRange("z_dist>0.");
  }



  SIPM_QE_setup::~SIPM_QE_setup()
  {
  }



  void SIPM_QE_setup::Construct()
  {
    // CHAMBER ///////////////////////////////////////////////////////

    // The parameterized width, height and length are internal dimensions.
    // The chamber thickness is added to obtain the external (total) size.
    G4double width = 5.*cm;
    G4double height = 5.*cm;
    length_ = 5.*cm;
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
    G4Box* gas_solid = new G4Box("GAS", width/2., height/2., length_/2.);

    // // xenon
    // G4double gxe_pressure = 10*bar;
    // G4Material* gxe = MaterialsList::GXe(gxe_pressure);
    // gxe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(gxe_pressure));
    // G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");

    // air
    G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    air->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());
    G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, air, "GAS");
    new G4PVPlacement(0, G4ThreeVector(0,0,0), gas_logic, "GAS",
		      chamber_logic, false, 0, true);

    // SIPM /////////////////////////////////////////
    sipm_.Construct();

    G4LogicalVolume* sipm_logic = sipm_.GetLogicalVolume();
    sipm_length_ = 1.3*mm;

    new G4PVPlacement(0, G4ThreeVector(0.,0.,-length_/2.+sipm_length_/2.),
		      sipm_logic, "SIPM",
		      gas_logic, false, 0, true);
  }



  G4ThreeVector SIPM_QE_setup::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector point;

    if (region == "CIRCLE"){
      G4double diameter = 20.*mm;
      G4double r_max = diameter/2.;
      G4double r = G4UniformRand()*r_max;
      G4double phi = G4UniformRand()*2*pi;
      G4double x = r*cos(phi);
      G4double y = r*sin(phi);
      point =  G4ThreeVector(x, y,-length_/2.+sipm_length_+z_dist_);

    } else if (region == "POINT") {
      point = G4ThreeVector(0, 0, -length_/2.+sipm_length_+z_dist_);
    }

    return point;
  }


} //end namespace nexus
