// ----------------------------------------------------------------------------
// nexus | MonolithicCsI.cc
//
// This class describes a pair of MonolithicCsI crystals with a radioactive source in the middle
//
// ----------------------------------------------------------------------------

#include "MonolithicCsI.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "OpticalMaterialProperties.h"
#include "SiPM66NoCasing.h"
#include "SiPM33NoCasing.h"

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

REGISTER_CLASS(MonolithicCsI, GeometryBase)

namespace nexus
{

  using namespace CLHEP;

  MonolithicCsI::MonolithicCsI() : GeometryBase(),
                                   crystal_width_(48 * mm),
                                   crystal_length_(22.8 * mm)
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/MonolithicCsI/",
                                  "Control commands of geometry MonolithicCsI.");
    msg_->DeclareProperty("crystal_width", crystal_width_, "Crystal width");
    msg_->DeclareProperty("crystal_length", crystal_length_, "Crystal length");
  }

  MonolithicCsI::~MonolithicCsI()
  {
    delete msg_;
  }

  void MonolithicCsI::Construct()
  {
    box_source_ = new BoxPointSampler(crystal_width_, crystal_width_, crystal_length_, 0, G4ThreeVector(0, 0, +25. / 2 * mm + crystal_length_ / 2));
    G4double size = 0.5 * m;
    G4Box *lab_solid =
        new G4Box("LAB", size / 2., size / 2., size / 2.);

    G4Material *air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

    G4LogicalVolume *lab_logic = new G4LogicalVolume(lab_solid, air, "LAB");

    lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    G4VPhysicalVolume *lab_phys = new G4PVPlacement(
            0, G4ThreeVector(), lab_logic, "lab", 0, false, 0, true);

    // Set this volume as the wrapper for the whole geometry
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(lab_logic);

    G4Box *crystal =
        new G4Box("CRYSTAL", crystal_width_ / 2., crystal_width_ / 2., crystal_length_ / 2.);

    // G4Material *CsI = G4NistManager::Instance()->FindOrBuildMaterial("G4_BGO");
    G4Material *CsI = materials::LYSO();
    CsI->SetMaterialPropertiesTable(opticalprops::LYSO());
    G4LogicalVolume *crystal_logic =
        new G4LogicalVolume(crystal,
                            CsI,
                            "CRYSTAL");
    crystal_logic->SetVisAttributes(nexus::LightBlueAlpha());
    G4VPhysicalVolume *crystal_right = new G4PVPlacement(0, G4ThreeVector(0, 0, +25. / 2 * mm + crystal_length_ / 2),
                                                         crystal_logic, "CRYSTAL", lab_logic,
                                                         true, 2, true);

    G4double teflon_thickness = 0.08 * mm;
    G4int teflon_coatings = 5;
    G4double teflon_thickness_tot = teflon_thickness * teflon_coatings;
    G4Box *teflon_coating_full = new G4Box("TEFLON_FULL", crystal_width_ / 2 + teflon_thickness_tot / 2, crystal_width_ / 2 + teflon_thickness_tot / 2, crystal_length_ / 2.);
    G4Box *teflon_back = new G4Box("TEFLON_BACK", crystal_width_ / 2 + teflon_thickness_tot / 2, crystal_width_ / 2 + teflon_thickness_tot / 2, teflon_thickness_tot / 2);

    G4SubtractionSolid *teflon_coating = new G4SubtractionSolid("TEFLON", teflon_coating_full, crystal);
    G4LogicalVolume *teflon_logic =
        new G4LogicalVolume(teflon_coating,
                            G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
                            "TEFLON_SIDES");
    G4LogicalVolume *teflon_back_logic =
        new G4LogicalVolume(teflon_back,
                            G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
                            "TEFLON_BACK");

    teflon_logic->SetVisAttributes(nexus::White());
    teflon_back_logic->SetVisAttributes(nexus::White());

    G4VPhysicalVolume *teflon_full_position = new G4PVPlacement(0, G4ThreeVector(0, 0, 25. / 2 * mm + crystal_length_ / 2),
                                                                teflon_logic, "TEFLON_SIDES", lab_logic,
                                                                true, 1, true);

    G4VPhysicalVolume* teflon_back_position = new G4PVPlacement(0, G4ThreeVector(0, 0, 25./2 * mm - teflon_thickness_tot/2 ),
                      teflon_back_logic, "TEFLON_BACK", lab_logic,
                      true, 2, true);

    G4OpticalSurface *ptfe_surface = new G4OpticalSurface("PTFE_SURFACE");
    ptfe_surface->SetType(dielectric_LUTDAVIS);
    ptfe_surface->SetFinish(RoughESR_LUT);
    // ptfe_surface->SetFinish(RoughTeflon_LUT);
    ptfe_surface->SetModel(DAVIS);
    // ptfe_surface->SetType(dielectric_dielectric);
    // ptfe_surface->SetFinish(polishedvm2000air);
    // ptfe_surface->SetFinish(polishedfrontpainted);
    // ptfe_surface->SetModel(unified);
    // ptfe_surface->SetMaterialPropertiesTable(opticalprops::PTFE());
//
    // G4OpticalSurface *air_surface = new G4OpticalSurface("CRYSTAL_SURFACE");
    // ptfe_surface->SetType(dielectric_LUTDAVIS);
    // ptfe_surface->SetFinish(Polished_LUT);
    // ptfe_surface->SetModel(DAVIS);

    new G4LogicalBorderSurface(
        "CRYSTAL_PTFE", crystal_right, teflon_full_position, ptfe_surface);

    new G4LogicalBorderSurface(
      "CRYSTAL_PTFE_BACK", crystal_right, teflon_back_position, ptfe_surface);

    SiPM66NoCasing *sipm_geom = new SiPM66NoCasing();

    sipm_geom->Construct();
    G4LogicalVolume *sipm_logic = sipm_geom->GetLogicalVolume();
    G4int n_rows = (int)crystal_width_ / sipm_geom->GetDimensions().x();
    G4int n_cols = (int)crystal_width_ / sipm_geom->GetDimensions().y();
    for (G4int irow = 0; irow < n_rows; irow++)
    {
      for (G4int icol = 0; icol < n_cols; icol++)
      {
        std::string label = std::to_string(irow * n_rows + icol);
        new G4PVPlacement(0, G4ThreeVector(irow * sipm_geom->GetDimensions().x() - crystal_width_ / 2 + sipm_geom->GetDimensions().x() / 2, icol * sipm_geom->GetDimensions().x() - crystal_width_ / 2 + sipm_geom->GetDimensions().x() / 2, 25. / 2 * mm + crystal_length_ + sipm_geom->GetDimensions().z() / 2), sipm_logic,
                          "SiPM" + label, lab_logic, true, irow * n_rows + icol);
      }
    }
  }

  G4ThreeVector MonolithicCsI::GenerateVertex(const G4String &region) const
  {
    G4ThreeVector vertex(0, 0, 0);
    // return vertex;
    return box_source_->GenerateVertex("INSIDE");
  }

}
