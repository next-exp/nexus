// ----------------------------------------------------------------------------
//  $Id: PetBox.cc  $
//
//  Author:  <paola.ferrario@dipc.org>
//  Created: January 2019
//
//  Copyright (c) 2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PetBox.h"
//#include "TileHamamatsuVUV.h"
#include "TileFBK.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "IonizationSD.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

namespace nexus {

  PetBox::PetBox(): BaseGeometry(),
                    visibility_(0),
                    source_pos_x_(0.*mm),
                    source_pos_y_(0.*mm),
                    source_pos_z_(0.*mm),
                    box_size_(19.2*cm),
                    box_thickness_(2.*cm),
                    ih_x_size_(4.6*cm),
                    ih_y_size_(12.*cm),
                    ih_z_size_(6*cm),
                    ih_thickness_(3.*mm),
                    source_tube_thickness_(1.*mm),
                    source_tube_int_radius_(1.1*cm),
                    source_tube_length_(10.*cm)
  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetBox/",
                                  "Control commands of geometry PetBox.");
    msg_->DeclareProperty("visibility", visibility_, "Visibility");

    G4GenericMessenger::Command& source_pos_x_cmd =
      msg_->DeclareProperty("source_pos_x", source_pos_x_, "X position of the source");
    source_pos_x_cmd.SetUnitCategory("Length");
    source_pos_x_cmd.SetParameterName("source_pos_x", false);

    G4GenericMessenger::Command& source_pos_y_cmd =
      msg_->DeclareProperty("source_pos_y", source_pos_y_, "Y position of the source");
    source_pos_y_cmd.SetUnitCategory("Length");
    source_pos_y_cmd.SetParameterName("source_pos_y", false);

    G4GenericMessenger::Command& source_pos_z_cmd =
      msg_->DeclareProperty("source_pos_z", source_pos_z_, "Z position of the source");
    source_pos_z_cmd.SetUnitCategory("Length");
    source_pos_z_cmd.SetParameterName("source_pos_z", false);


    G4GenericMessenger::Command& box_size_cmd =
      msg_->DeclareProperty("box_size", box_size_, "Size of box");
    box_size_cmd.SetUnitCategory("Length");
    box_size_cmd.SetParameterName("box_size", false);
    box_size_cmd.SetRange("box_size>0.");

    G4GenericMessenger::Command& box_thickness_cmd =
      msg_->DeclareProperty("box_thickness", box_thickness_, "Thickness of box");
    box_thickness_cmd.SetUnitCategory("Length");
    box_thickness_cmd.SetParameterName("box_thickness", false);
    box_thickness_cmd.SetRange("box_thickness>0.");

    G4GenericMessenger::Command& ih_x_cmd =
      msg_->DeclareProperty("ih_x", ih_x_size_, "X size of internal hat");
    ih_x_cmd.SetUnitCategory("Length");
    ih_x_cmd.SetParameterName("ih_x", false);
    ih_x_cmd.SetRange("ih_x>0.");

    G4GenericMessenger::Command& ih_y_cmd =
      msg_->DeclareProperty("ih_y", ih_y_size_, "Y size of internal hat");
    ih_y_cmd.SetUnitCategory("Length");
    ih_y_cmd.SetParameterName("ih_y", false);
    ih_y_cmd.SetRange("ih_y>0.");

    G4GenericMessenger::Command& ih_z_cmd =
      msg_->DeclareProperty("ih_z", ih_z_size_, "Z size of internal hat");
    ih_z_cmd.SetUnitCategory("Length");
    ih_z_cmd.SetParameterName("ih_z", false);
    ih_z_cmd.SetRange("ih_z>0.");

    G4GenericMessenger::Command& ih_thickness_cmd =
      msg_->DeclareProperty("ih_thickness", ih_thickness_, "Thickness of internal hat");
    ih_thickness_cmd.SetUnitCategory("Length");
    ih_thickness_cmd.SetParameterName("ih_thickness", false);
    ih_thickness_cmd.SetRange("ih_thickness>0.");

    G4GenericMessenger::Command& source_tube_thickness_cmd =
      msg_->DeclareProperty("source_tube_thickness", source_tube_thickness_, "Thickness of source_tube");
    source_tube_thickness_cmd.SetUnitCategory("Length");
    source_tube_thickness_cmd.SetParameterName("source_tube_thickness", false);
    source_tube_thickness_cmd.SetRange("source_tube_thickness>0.");

    G4GenericMessenger::Command& source_tube_int_radius_cmd =
      msg_->DeclareProperty("source_tube_int_radius", source_tube_int_radius_, "Int_Radius of source_tube");
    source_tube_int_radius_cmd.SetUnitCategory("Length");
    source_tube_int_radius_cmd.SetParameterName("source_tube_int_radius", false);
    source_tube_int_radius_cmd.SetRange("source_tube_int_radius>0.");

    G4GenericMessenger::Command& source_tube_length_cmd =
      msg_->DeclareProperty("source_tube_length", source_tube_length_, "Length of source_tube");
    source_tube_length_cmd.SetUnitCategory("Length");
    source_tube_length_cmd.SetParameterName("source_tube_length", false);
    source_tube_length_cmd.SetRange("source_tube_length>0.");


    tile_ = new TileFBK();
  }

  PetBox::~PetBox()
  {
  }

  void PetBox::Construct()
  {
    // LAB. Volume of air surrounding the detector
    G4double lab_size = 1.*m;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

    lab_logic_ =
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic_);

     BuildBox();
     BuildSensors();
  }

  void PetBox::BuildBox()
  {

    // BOX
    G4Box* box_solid =
      new G4Box("BOX", box_size_/2., box_size_/2., box_size_/2.);

    G4Material* aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
    G4LogicalVolume* box_logic =
      new G4LogicalVolume(box_solid, aluminum, "BOX");

    new G4PVPlacement(0, G4ThreeVector(0., 0, 0), box_logic,
                      "BOX", lab_logic_, false, 0, true);

    // LXe
    G4double LXe_size = box_size_ - 2.*box_thickness_;
    G4Box* LXe_solid =
      new G4Box("LXe", LXe_size/2., LXe_size/2., LXe_size/2.);

    G4Material* LXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    LXe->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    LXe_logic_ =
      new G4LogicalVolume(LXe_solid, LXe, "LXE");

    new G4PVPlacement(0, G4ThreeVector(0., 0, 0), LXe_logic_,
                      "LXE", box_logic, false, 0, true);


    // INTERNAL HAT AND SOURCE TUBE
    G4Box* internal_hat_solid =
      new G4Box("INTERNAL_HAT", ih_x_size_/2., ih_y_size_/2., ih_z_size_/2.);

    G4LogicalVolume* internal_hat_logic =
      new G4LogicalVolume(internal_hat_solid, aluminum, "INTERNAL_HAT");

    new G4PVPlacement(0, G4ThreeVector(0., (-box_size_/2.+box_thickness_+ih_y_size_/2.), 0), internal_hat_logic,
                      "INTERNAL_HAT", LXe_logic_, false, 0, true);

    G4Box* vacuum_hat_solid =
      new G4Box("VACUUM_HAT", (ih_x_size_-2.*ih_thickness_)/2., (ih_y_size_-2.*ih_thickness_)/2., (ih_z_size_-2.*ih_thickness_)/2.);

    G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");;
    vacuum->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());
    G4LogicalVolume* vacuum_hat_logic =
      new G4LogicalVolume(vacuum_hat_solid, vacuum, "VACUUM_HAT");

    new G4PVPlacement(0, G4ThreeVector(0., 0, 0), vacuum_hat_logic,
                      "VACUUM_HAT", internal_hat_logic, false, 0, true);

    G4Tubs* source_tube_solid =
      new G4Tubs("SOURCE_TUBE", 0, source_tube_int_radius_ + source_tube_thickness_, source_tube_length_/2., 0, twopi);

    G4Material* carbon_fiber = MaterialsList::CarbonFiber();
    G4LogicalVolume* source_tube_logic =
      new G4LogicalVolume(source_tube_solid, carbon_fiber, "SOURCE_TUBE");

    G4RotationMatrix rot;
    rot.rotateX(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., 0., 0.)), source_tube_logic,
                      "SOURCE_TUBE", vacuum_hat_logic, false, 0, true);

    G4Tubs* air_source_tube_solid =
      new G4Tubs("AIR_SOURCE_TUBE", 0, source_tube_int_radius_, (source_tube_length_ - 2.*source_tube_thickness_)/2., 0, twopi);

    G4LogicalVolume* air_source_tube_logic =
      new G4LogicalVolume(air_source_tube_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "AIR_SOURCE_TUBE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), air_source_tube_logic,
                      "AIR_SOURCE_TUBE", source_tube_logic, false, 0, true);


    // TILE CONSTRUCT
    G4double n_tile_rows = 2;
    G4double n_tile_columns = 2;
    tile_->Construct();
    tile_thickn_ = tile_->GetDimensions().z();
    full_row_size_ = n_tile_columns * tile_->GetDimensions().x();
    full_col_size_ = n_tile_rows * tile_->GetDimensions().y();



    // Visibilities
    if (visibility_) {
      G4VisAttributes box_col = nexus::White();
      box_logic->SetVisAttributes(box_col);
      G4VisAttributes lxe_col = nexus::Blue();
      //lxe_col.SetForceSolid(true);
      LXe_logic_->SetVisAttributes(lxe_col);
      G4VisAttributes ih_col = nexus::Yellow();
      internal_hat_logic->SetVisAttributes(ih_col);
      G4VisAttributes vacuum_col = nexus::Lilla();
      vacuum_hat_logic->SetVisAttributes(vacuum_col);
      G4VisAttributes source_tube_col = nexus::Red();
      //source_tube_col.SetForceSolid(true);
      source_tube_logic->SetVisAttributes(source_tube_col);
      G4VisAttributes air_source_tube_col = nexus::DarkGrey();
      air_source_tube_logic->SetVisAttributes(air_source_tube_col);
    }
    else {
      box_logic->SetVisAttributes(G4VisAttributes::Invisible);
      LXe_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  void PetBox::BuildSensors()
  {
  }


  G4ThreeVector PetBox::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "CENTER") {
      return vertex;
    } else if (region == "AD_HOC") {
      vertex = G4ThreeVector(source_pos_x_, source_pos_y_, source_pos_z_);
    } else {
      G4Exception("[PetBox]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");
    }
    return vertex;
  }

 }
