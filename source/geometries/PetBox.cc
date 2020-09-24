// ----------------------------------------------------------------------------
// nexus | PetBox.cc
//
// This class implements the geometry of a symmetric box of LXe.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PetBox.h"
#include "TileHamamatsuVUV.h"
//#include "TileHamamatsuBlue.h"
//#include "TileFBK.h"
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
                    reflectivity_(0),
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
                    source_tube_length_(11.*cm),
                    active_depth_(3.*cm),
                    dist_dice_panel_(5.25*mm),
                    dist_ihat_panel_(2.*mm),
                    panel_thickness_(1.75*mm),
                    max_step_size_(1.*mm)
  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetBox/",
                                  "Control commands of geometry PetBox.");
    msg_->DeclareProperty("visibility", visibility_, "Visibility");
    msg_->DeclareProperty("surf_reflectivity", reflectivity_, "Reflectivity of the panels");

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

    tile_ = new TileHamamatsuVUV();
  }

  PetBox::~PetBox()
  {
  }

  void PetBox::Construct()
  {
    // LAB. Volume of air surrounding the detector
    G4double lab_size = 1.*m;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

    G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    lab_logic_ = new G4LogicalVolume(lab_solid, air, "LAB");
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
                      "BOX", lab_logic_, false, 0, false);

    // LXe
    G4double LXe_size = box_size_ - 2.*box_thickness_;
    G4Box* LXe_solid =
      new G4Box("LXe", LXe_size/2., LXe_size/2., LXe_size/2.);

    G4Material* LXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    LXe->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    LXe_logic_ =
      new G4LogicalVolume(LXe_solid, LXe, "LXE");

    new G4PVPlacement(0, G4ThreeVector(0., 0, 0), LXe_logic_,
                      "LXE", box_logic, false, 0, false);


    // INTERNAL HAT
    G4Box* internal_hat_solid =
      new G4Box("INTERNAL_HAT", ih_x_size_/2., ih_y_size_/2., ih_z_size_/2.);

    G4LogicalVolume* internal_hat_logic =
      new G4LogicalVolume(internal_hat_solid, aluminum, "INTERNAL_HAT");

    new G4PVPlacement(0, G4ThreeVector(0., (-box_size_/2.+box_thickness_+ih_y_size_/2.), 0),
      internal_hat_logic, "INTERNAL_HAT", LXe_logic_, false, 0, false);

    G4double vacuum_hat_xsize = ih_x_size_-2.*ih_thickness_;
    G4double vacuum_hat_ysize = ih_y_size_-2.*ih_thickness_;
    G4double vacuum_hat_zsize = ih_z_size_-2.*ih_thickness_;
    G4Box* vacuum_hat_solid =
      new G4Box("VACUUM_HAT", vacuum_hat_xsize/2.,vacuum_hat_ysize/2., vacuum_hat_zsize/2.);

    G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");;
    vacuum->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());
    G4LogicalVolume* vacuum_hat_logic =
      new G4LogicalVolume(vacuum_hat_solid, vacuum, "VACUUM_HAT");

    new G4PVPlacement(0, G4ThreeVector(0., 0, 0), vacuum_hat_logic,
                      "VACUUM_HAT", internal_hat_logic, false, 0, false);

    // SOURCE TUBE
    G4double source_tube_ext_radius = source_tube_int_radius_ + source_tube_thickness_;
    G4Tubs* source_tube_solid =
      new G4Tubs("SOURCE_TUBE", 0, source_tube_ext_radius, source_tube_length_/2., 0, twopi);

    G4Material* carbon_fiber = MaterialsList::CarbonFiber();
    G4LogicalVolume* source_tube_logic =
      new G4LogicalVolume(source_tube_solid, carbon_fiber, "SOURCE_TUBE");

    G4RotationMatrix rot;
    rot.rotateX(pi/2.);
    G4double source_tube_ypos = source_tube_length_/2.-(ih_y_size_-2.*ih_thickness_)/2.;
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., source_tube_ypos, 0.)),
      source_tube_logic,"SOURCE_TUBE", vacuum_hat_logic, false, 0, false);

    G4double air_source_tube_len = source_tube_length_/2.-source_tube_thickness_;
    G4Tubs* air_source_tube_solid =
      new G4Tubs("AIR_SOURCE_TUBE", 0, source_tube_int_radius_, air_source_tube_len, 0, twopi);

    G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    G4LogicalVolume* air_source_tube_logic =
      new G4LogicalVolume(air_source_tube_solid, air, "AIR_SOURCE_TUBE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., source_tube_thickness_), air_source_tube_logic,
                      "AIR_SOURCE_TUBE", source_tube_logic, false, 0, false);

    // SOURCE TUBE INSIDE HAT
    G4Tubs* source_tube_inside_hat_solid =
      new G4Tubs("SOURCE_TUBE", 0, source_tube_ext_radius, ih_thickness_/2., 0, twopi);

    G4LogicalVolume* source_tube_inside_hat_logic =
      new G4LogicalVolume(source_tube_inside_hat_solid, carbon_fiber, "SOURCE_TUBE");

    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., (-ih_y_size_+ih_thickness_)/2., 0.)),
                      source_tube_inside_hat_logic, "SOURCE_TUBE", internal_hat_logic, false, 0, false);

    G4Tubs* air_source_tube_inside_hat_solid =
      new G4Tubs("AIR_SOURCE_TUBE", 0, source_tube_int_radius_, ih_thickness_/2., 0, twopi);

    G4LogicalVolume* air_source_tube_inside_hat_logic =
      new G4LogicalVolume(air_source_tube_inside_hat_solid, air, "AIR_SOURCE_TUBE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), air_source_tube_inside_hat_logic,
                      "AIR_SOURCE_TUBE", source_tube_inside_hat_logic, false, 0, false);

    // SOURCE TUBE INSIDE BOX
    G4Tubs* source_tube_inside_box_solid =
      new G4Tubs("SOURCE_TUBE", 0, source_tube_ext_radius, box_thickness_/2., 0, twopi);

    G4LogicalVolume* source_tube_inside_box_logic =
      new G4LogicalVolume(source_tube_inside_box_solid, carbon_fiber, "SOURCE_TUBE");

    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., (-box_size_+box_thickness_)/2., 0.)),
                      source_tube_inside_box_logic, "SOURCE_TUBE", box_logic, false, 0, false);

    G4Tubs* air_source_tube_inside_box_solid =
      new G4Tubs("AIR_SOURCE_TUBE", 0, source_tube_int_radius_, box_thickness_/2., 0, twopi);

    G4LogicalVolume* air_source_tube_inside_box_logic =
      new G4LogicalVolume(air_source_tube_inside_box_solid, air, "AIR_SOURCE_TUBE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), air_source_tube_inside_box_logic,
                      "AIR_SOURCE_TUBE", source_tube_inside_box_logic, false, 0, false);



    // TILE CONSTRUCT
    n_tile_rows_ = 2;
    n_tile_columns_ = 2;
    tile_->Construct();
    tile_thickn_ = tile_->GetDimensions().z();
    full_row_size_ = n_tile_columns_ * tile_->GetDimensions().x();
    full_col_size_ = n_tile_rows_ * tile_->GetDimensions().y();


    // 2 ACTIVE REGIONS
    G4double active_y_size = full_col_size_ + 2.*dist_dice_panel_;
    G4double active_z_size = full_row_size_ + 2.*dist_dice_panel_;
    G4double active_x_pos = ih_x_size_/2. + dist_ihat_panel_ + panel_thickness_ + active_depth_/2.;

    G4Box* active_solid =
      new G4Box("ACTIVE", active_depth_/2., active_y_size/2., active_z_size/2.);
    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, LXe, "ACTIVE");
    new G4PVPlacement(0, G4ThreeVector(-active_x_pos, 0., 0.), active_logic,
                      "ACTIVE", LXe_logic_, false, 1, false);
    new G4PVPlacement(0, G4ThreeVector(active_x_pos, 0., 0.), active_logic,
                      "ACTIVE", LXe_logic_, false, 2, false);

    // Set the ACTIVE volume as an ionization sensitive det
    IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));


    // PYREX PANELS BETWEEN THE INTERNAL HAT AND THE ACTIVE REGIONS
    G4double entry_panel_width = full_row_size_ + 2* dist_dice_panel_;
    G4double entry_panel_xpos = ih_x_size_/2. + panel_thickness_/2. + dist_ihat_panel_;
    G4double entry_panel_ypos = -box_size_/2. + box_thickness_ + ih_y_size_/2.;

    G4Box* entry_panel_solid =
      new G4Box("ENTRY_PANEL", panel_thickness_/2., ih_y_size_/2., entry_panel_width/2.);

    G4Material* pyrex = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pyrex_Glass");

    G4LogicalVolume* entry_panel_logic =
      new G4LogicalVolume(entry_panel_solid, pyrex, "ENTRY_PANEL");

    new G4PVPlacement(0, G4ThreeVector(-entry_panel_xpos, entry_panel_ypos, 0), entry_panel_logic,
                        "ENTRY_PANEL", LXe_logic_, false, 1, false);

    new G4PVPlacement(0, G4ThreeVector(entry_panel_xpos, entry_panel_ypos, 0), entry_panel_logic,
                        "ENTRY_PANEL", LXe_logic_, false, 2, false);


    //PYREX PANELS SURROUNDING THE SIPM DICE BOARDS
    G4double lat_panel_width = active_depth_ + tile_thickn_;
    G4double horiz_panel_length = full_row_size_ + 2.*dist_dice_panel_;
    G4double lat_panel_xpos = ih_x_size_/2. + dist_ihat_panel_ + panel_thickness_
                              + active_depth_ + tile_thickn_ - lat_panel_width/2.;
    G4double lat_panel_ypos = full_col_size_/2. + dist_dice_panel_ + panel_thickness_/2.;

    G4Box* horiz_lat_panel_solid =
      new G4Box("LAT_PANEL", lat_panel_width/2., panel_thickness_/2., horiz_panel_length/2.);

    G4LogicalVolume* horiz_lat_panel_logic =
      new G4LogicalVolume(horiz_lat_panel_solid, pyrex, "LAT_PANEL");

    new G4PVPlacement(0, G4ThreeVector(-lat_panel_xpos, lat_panel_ypos, 0),
                      horiz_lat_panel_logic, "LAT_PANEL", LXe_logic_, false, 1, false);

    new G4PVPlacement(0, G4ThreeVector(-lat_panel_xpos, -lat_panel_ypos, 0),
                      horiz_lat_panel_logic, "LAT_PANEL", LXe_logic_, false, 2, false);

    new G4PVPlacement(0, G4ThreeVector(lat_panel_xpos, lat_panel_ypos, 0), horiz_lat_panel_logic,
                      "LAT_PANEL", LXe_logic_, false, 3, false);

    new G4PVPlacement(0, G4ThreeVector(lat_panel_xpos, -lat_panel_ypos, 0), horiz_lat_panel_logic,
                      "LAT_PANEL", LXe_logic_, false, 4, false);


    G4double vert_panel_length = full_col_size_ + 2.*dist_dice_panel_;
    G4double lat_panel_zpos = full_row_size_/2. + dist_dice_panel_ + panel_thickness_/2.;

    G4Box* vert_lat_panel_solid =
      new G4Box("LAT_PANEL", lat_panel_width/2., vert_panel_length/2., panel_thickness_/2.);

    G4LogicalVolume* vert_lat_panel_logic =
      new G4LogicalVolume(vert_lat_panel_solid, pyrex, "LAT_PANEL");

    new G4PVPlacement(0, G4ThreeVector(-lat_panel_xpos, 0, -lat_panel_zpos),
                      vert_lat_panel_logic, "LAT_PANEL", LXe_logic_, false, 1, false);

    new G4PVPlacement(0, G4ThreeVector(-lat_panel_xpos, 0, lat_panel_zpos),
                      vert_lat_panel_logic, "LAT_PANEL", LXe_logic_, false, 2, false);

    new G4PVPlacement(0, G4ThreeVector(lat_panel_xpos, 0, -lat_panel_zpos),
                      vert_lat_panel_logic, "LAT_PANEL", LXe_logic_, false, 3, false);

    new G4PVPlacement(0, G4ThreeVector(lat_panel_xpos, 0, lat_panel_zpos),
                      vert_lat_panel_logic, "LAT_PANEL", LXe_logic_, false, 4, false);


    // Optical surface for the panels
    G4OpticalSurface* panel_opsur = new G4OpticalSurface("OP_PANEL");
    panel_opsur->SetType(dielectric_metal);
    panel_opsur->SetModel(unified);
    panel_opsur->SetFinish(ground);
    panel_opsur->SetSigmaAlpha(0.1);
    panel_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::ReflectantSurface(reflectivity_));
    new G4LogicalSkinSurface("OP_PANEL", entry_panel_logic, panel_opsur);
    new G4LogicalSkinSurface("OP_PANEL_H", horiz_lat_panel_logic, panel_opsur);
    new G4LogicalSkinSurface("OP_PANEL_V", vert_lat_panel_logic, panel_opsur);


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
      source_tube_col.SetForceSolid(true);
      source_tube_logic->SetVisAttributes(source_tube_col);
      G4VisAttributes air_source_tube_col = nexus::DarkGrey();
      //air_source_tube_col.SetForceSolid(true);
      air_source_tube_logic->SetVisAttributes(air_source_tube_col);
      G4VisAttributes air_source_tube_inside_hat_col = nexus::Blue();
      air_source_tube_inside_hat_col.SetForceSolid(true);
      source_tube_inside_hat_logic->SetVisAttributes(air_source_tube_inside_hat_col);
      G4VisAttributes air_source_tube_inside_box_col = nexus::White();
      air_source_tube_inside_box_col.SetForceSolid(true);
      source_tube_inside_box_logic->SetVisAttributes(air_source_tube_inside_box_col);
      G4VisAttributes active_col = nexus::Blue();
      active_logic->SetVisAttributes(active_col);
      G4VisAttributes panel_col = nexus::Red();
      entry_panel_logic->SetVisAttributes(panel_col);
    }
    else {
      box_logic->SetVisAttributes(G4VisAttributes::Invisible);
      LXe_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  void PetBox::BuildSensors()
  {
    G4LogicalVolume* tile_logic = tile_->GetLogicalVolume();
    G4double x_pos = ih_x_size_/2. + dist_ihat_panel_ + panel_thickness_ + active_depth_ + tile_thickn_/2.;

    G4RotationMatrix rot;
    rot.rotateY(-pi/2.);
    G4ThreeVector position;
    G4String vol_name;

    G4int copy_no = 0;

    G4double tile_size_x = tile_->GetDimensions().x();
    G4double tile_size_y = tile_->GetDimensions().y();
    for (G4int i=0; i<n_tile_rows_; i++) {
      G4double y_pos = full_col_size_/2. - tile_size_y/2. - i*tile_size_y;
      for (G4int j=0; j<n_tile_columns_; j++) {
        G4double z_pos = -full_row_size_/2. + tile_size_x/2. + j*tile_size_x;
        position = G4ThreeVector(x_pos, y_pos, z_pos);
        vol_name = "TILE_" + std::to_string(copy_no);
        new G4PVPlacement(G4Transform3D(rot, position), tile_logic,
                          vol_name, LXe_logic_, false, copy_no, false);
        copy_no += 1;
      }
    }

    rot.rotateY(pi);

    for (G4int i=0; i<n_tile_rows_; i++) {
      G4double y_pos = full_col_size_/2. - tile_size_y/2. - i*tile_size_y;
      for (G4int j=0; j<n_tile_columns_; j++) {
        G4double z_pos = full_row_size_/2. - tile_size_x/2. - j*tile_size_x;
        position = G4ThreeVector(-x_pos, y_pos, z_pos);
        vol_name = "TILE_" + std::to_string(copy_no);
        new G4PVPlacement(G4Transform3D(rot, position), tile_logic,
                          vol_name, LXe_logic_, false, copy_no, false);
        copy_no += 1;
      }
    }
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
