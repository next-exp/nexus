// ----------------------------------------------------------------------------
// nexus | CsI.cc
//
// This class describes a pair of CsI crystals with a radioactive source in the middle
//
// ----------------------------------------------------------------------------

#include "CsI.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "OpticalMaterialProperties.h"
#include "SiPM33.h"
#include "SiPM66.h"

#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>

#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>

#include "FactoryBase.h"


#include <CLHEP/Units/SystemOfUnits.h>

using namespace nexus;

REGISTER_CLASS(CsI, GeometryBase)

namespace nexus {

  using namespace CLHEP;

  CsI::CsI():
    GeometryBase(),
    crystal_width_(3*mm),
    crystal_length_(20*mm)
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/CsI/",
                                  "Control commands of geometry CsI.");
    msg_->DeclareProperty("crystal_width", crystal_width_, "Crystal width");
    msg_->DeclareProperty("crystal_length", crystal_length_, "Crystal length");
  }

  CsI::~CsI()
  {
    delete msg_;
  }


  void CsI::Construct()
  {
    inside_source_ = new CylinderPointSampler2020(0, 0.5*mm / 2,  2.5 * mm / 2,  0, twopi, nullptr, G4ThreeVector(0, 0, 0));
    box_source_ = new BoxPointSampler(crystal_width_, crystal_width_, crystal_length_, 0.1*mm, G4ThreeVector(-1 * cm, 0, crystal_length_/2+2.5/2*cm));

    G4double size = 0.5 * m;
    G4Box* lab_solid =
      new G4Box("LAB", size/2., size/2., size/2.);

    G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

    G4LogicalVolume* lab_logic = new G4LogicalVolume(lab_solid, air, "LAB");

    lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

    // Set this volume as the wrapper for the whole geometry
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(lab_logic);

    G4Box* crystal =
      new G4Box("CRYSTAL", crystal_width_/2., crystal_width_/2., crystal_length_/2.);

    G4Tubs* source = new G4Tubs("SOURCE", 0, 15.*mm /2, 0.5 * mm /2, 0, 2*pi );
    G4LogicalVolume* source_logic =
      new G4LogicalVolume(source,
                          G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYCARBONATE"),
                          "SOURCE");
    G4Tubs* ring = new G4Tubs("RING", 15.*mm/2, 25*mm /2 - 400 * um, 3 * mm /2, 0, 2*pi );
    G4LogicalVolume* ring_logic =
      new G4LogicalVolume(ring,
                          G4NistManager::Instance()->FindOrBuildMaterial("G4_PLEXIGLASS"),
                          "SOURCE");
    ring_logic->SetVisAttributes(nexus::LightBlueAlpha());
    G4RotationMatrix *rot_z = new G4RotationMatrix();
      rot_z->rotateX(90 * deg);
    new G4PVPlacement(G4Transform3D(*rot_z, G4ThreeVector(0, 0, 0)),
                      source_logic, "SOURCE", lab_logic,
                      true, 1, true);
    new G4PVPlacement(G4Transform3D(*rot_z, G4ThreeVector(0, 0, 0)),
                      ring_logic, "RING", lab_logic,
                      true, 1, true);
    G4Material *csi = G4NistManager::Instance()->FindOrBuildMaterial("G4_CESIUM_IODIDE");
    csi->SetMaterialPropertiesTable(opticalprops::CsI());
    G4LogicalVolume* crystal_logic =
      new G4LogicalVolume(crystal,
                          csi,
                          "CRYSTAL");
    crystal_logic->SetVisAttributes(nexus::LightGreyAlpha());
    new G4PVPlacement(0, G4ThreeVector(0, 0, -25./2 * mm - crystal_length_/2),
                      crystal_logic, "CRYSTAL_LEFT", lab_logic,
                      true, 1, true);
    G4VPhysicalVolume *crystal_right = new G4PVPlacement(0, G4ThreeVector(0, 0, +25./2 * mm + crystal_length_/2),
                      crystal_logic, "CRYSTAL_RIGHT", lab_logic,
                      true, 2, true);

    G4double teflon_thickness = 0.08 * mm;
    G4int teflon_coatings = 5;
    G4double teflon_thickness_tot = teflon_thickness * teflon_coatings;
    G4Box *teflon_coating_full = new G4Box("TEFLON_FULL", crystal_width_ / 2+ teflon_thickness_tot / 2, crystal_width_ / 2+ teflon_thickness_tot / 2, crystal_length_ /2.);
    G4Box *copper = new G4Box("COPPER_HOLDER", crystal_width_ / 2+ teflon_thickness_tot / 2 + 1 * mm, crystal_width_ / 2+ teflon_thickness_tot / 2 + 1 * mm, crystal_length_ /2.);
    G4Tubs *copper_plate = new G4Tubs("COPPER_PLATE", 0, 5 * cm, 3 / 2. * mm, 0, 2*pi);
    G4LogicalVolume* copper_plate_logic =
      new G4LogicalVolume(copper_plate,
                          G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
                          "COPPER_PLATE");
    copper_plate_logic->SetVisAttributes(nexus::CopperBrown());
    if (crystal_width_ == 3 * mm) {
      new G4PVPlacement(G4Transform3D(*rot_z, G4ThreeVector(0, - 4.5 * mm, 0)),
                        copper_plate_logic, "COPPER_PLATE", lab_logic,
                        false, 0, false);
    } else {
      new G4PVPlacement(G4Transform3D(*rot_z, G4ThreeVector(0, - 6 * mm, 0)),
                        copper_plate_logic, "COPPER_PLATE", lab_logic,
                        false, 0, false);
    }
    G4Box *teflon_back = new G4Box("TEFLON_BACK", crystal_width_ / 2+ teflon_thickness_tot / 2, crystal_width_ / 2 + teflon_thickness_tot / 2, teflon_thickness_tot / 2);

    G4SubtractionSolid *teflon_coating = new G4SubtractionSolid("TEFLON",teflon_coating_full,crystal);
    G4SubtractionSolid *copper_holder_1 = new G4SubtractionSolid("COPPER",copper,crystal);
    G4SubtractionSolid *copper_holder = new G4SubtractionSolid("COPPER",copper_holder_1,teflon_coating_full);

    G4LogicalVolume* copper_logic =
    new G4LogicalVolume(copper_holder,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
                        "COPPER_HOLDER");
    copper_logic->SetVisAttributes(nexus::CopperBrownAlpha());

    G4LogicalVolume* teflon_logic =
    new G4LogicalVolume(teflon_coating,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
                        "TEFLON");
    G4LogicalVolume* teflon_back_logic =
    new G4LogicalVolume(teflon_back,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
                        "TEFLON_BACK");

    teflon_logic->SetVisAttributes(nexus::White());
    teflon_back_logic->SetVisAttributes(nexus::White());

    G4VPhysicalVolume* teflon_full_position = new G4PVPlacement(0, G4ThreeVector(0, 0, 25./2 * mm + crystal_length_/2),
                    teflon_logic, "TEFLON_RIGHT", lab_logic,
                    true, 1, true);
    G4VPhysicalVolume* copper_position = new G4PVPlacement(0, G4ThreeVector(0, 0, 25./2 * mm + crystal_length_/2),
                    copper_logic, "COPPER_RIGHT", lab_logic,
                    true, 1, true);

    G4VPhysicalVolume* teflon_back_position = new G4PVPlacement(0, G4ThreeVector(0, 0, 25./2 * mm - teflon_thickness_tot/2 ),
                      teflon_back_logic, "TEFLON_BACK", lab_logic,
                      true, 2, true);

    G4OpticalSurface* ptfe_surface = new G4OpticalSurface("PTFE_SURFACE");
    ptfe_surface->SetType(dielectric_LUT);
    ptfe_surface->SetFinish(polishedteflonair);
    ptfe_surface->SetModel(LUT);
    ptfe_surface->SetMaterialPropertiesTable(opticalprops::PTFE());

    new G4LogicalBorderSurface(
      "CRYSTAL_PTFE", crystal_right, teflon_full_position, ptfe_surface);

    new G4LogicalBorderSurface(
      "CRYSTAL_PTFE_BACK", crystal_right, teflon_back_position, ptfe_surface);

    G4Box *optical_coupler = new G4Box("OPT_COUPLER", crystal_width_ / 2 , crystal_width_ / 2 , 0.1 / 2 * mm);
    G4Material * opt_coupler_material = materials::OpticalSilicone();
    opt_coupler_material->SetMaterialPropertiesTable(opticalprops::OptCoupler());

    G4LogicalVolume* optical_coupler_logic =
    new G4LogicalVolume(optical_coupler,
                        opt_coupler_material,
                        "OPT_COUPLER");

    // new G4PVPlacement(0, G4ThreeVector(0,0,25./2 * mm + crystal_length_ + 0.1 / 2 * mm), optical_coupler_logic,
    //   "OPT_COUPLER1", lab_logic, false, 0);
    // new G4PVPlacement(0, G4ThreeVector(0,0,25./2 * mm - 0.1 / 2 * mm ), optical_coupler_logic,
    //   "OPT_COUPLER2", lab_logic, true, 1);


    if (crystal_width_ == 3 * mm) {
      SiPM33 *sipm_geom = new SiPM33();
      sipm_geom->Construct();
      G4LogicalVolume* sipm_logic = sipm_geom->GetLogicalVolume();
      new G4PVPlacement(0, G4ThreeVector(0,0,25./2 * mm + crystal_length_ + sipm_geom->GetDimensions().z() / 2), sipm_logic,
        "SIPM33_1", lab_logic, true, 0);
      // rot_z->rotateX(90 * deg);
      // new G4PVPlacement(G4Transform3D(*rot_z, G4ThreeVector(0,0,25./2 * mm - sipm_geom->GetDimensions().z() / 2 - 0.1 *mm )), sipm_logic,
      //   "SIPM33_2", lab_logic, true, 1);
    } else if (crystal_width_ == 6 * mm) {
      SiPM66 *sipm_geom = new SiPM66();
      sipm_geom->Construct();
      G4LogicalVolume* sipm_logic = sipm_geom->GetLogicalVolume();
      new G4PVPlacement(0, G4ThreeVector(0,0,25./2 * mm + crystal_length_ + sipm_geom->GetDimensions().z() / 2), sipm_logic,
        "SIPM66_1", lab_logic, true, 0);
      // rot_z->rotateX(90 * deg);
      // new G4PVPlacement(G4Transform3D(*rot_z, G4ThreeVector(0,0,25./2 * mm - sipm_geom->GetDimensions().z() / 2 - 0.1 *mm )), sipm_logic,
      //   "SIPM66_2", lab_logic, true, 1);
    }

  }

  G4ThreeVector CsI::GenerateVertex(const G4String& region) const
  {
    return inside_source_->GenerateVertex("VOLUME");
    // return box_source_->GenerateVertex("INSIDE");
  }

}
