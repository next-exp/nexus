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
                    source_tube_length_(10.*cm),
                    active_depth_(3.*cm),
                    dist_dice_walls_(5.25*mm),
                    dist_ihat_wall_(2.*mm),
                    wall_thickness_(1.75*mm),
                    max_step_size_(1.*mm)
  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetBox/",
                                  "Control commands of geometry PetBox.");
    msg_->DeclareProperty("visibility", visibility_, "Visibility");
    msg_->DeclareProperty("surf_reflectivity", reflectivity_, "Reflectivity of box walls");

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
    n_tile_rows_ = 2;
    n_tile_columns_ = 2;
    tile_->Construct();
    tile_thickn_ = tile_->GetDimensions().z();
    full_row_size_ = n_tile_columns_ * tile_->GetDimensions().x();
    full_col_size_ = n_tile_rows_ * tile_->GetDimensions().y();


    // 2 ACTIVE REGIONS
    G4double active_y_size = full_col_size_ + 2.*dist_dice_walls_;
    G4double active_z_size = full_row_size_ + 2.*dist_dice_walls_;
    G4double active_x_pos = ih_x_size_/2. + dist_ihat_wall_ + wall_thickness_ + active_depth_/2.;

    G4Box* active_solid =
      new G4Box("ACTIVE", active_depth_/2., active_y_size/2., active_z_size/2.);
    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, LXe, "ACTIVE");
    new G4PVPlacement(0, G4ThreeVector(-active_x_pos, 0., 0.), active_logic,
                      "ACTIVE", LXe_logic_, false, 1, true);
    new G4PVPlacement(0, G4ThreeVector(active_x_pos, 0., 0.), active_logic,
                      "ACTIVE", LXe_logic_, false, 2, true);

    // Set the ACTIVE volume as an ionization sensitive det
    IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));


    // PYREX WALLS BETWEEN THE INTERNAL HAT AND THE ACTIVE REGIONS
    G4double sep_wall_width = full_row_size_ + 2* dist_dice_walls_;
    G4double sep_wall_x_pos = ih_x_size_/2. + wall_thickness_/2. + dist_ihat_wall_;
    G4double sep_wall_y_pos = -box_size_/2. + box_thickness_ + ih_y_size_/2.;

    G4Box* separation_wall_solid =
      new G4Box("SEPARATION_WALL", wall_thickness_/2., ih_y_size_/2., sep_wall_width/2.);

    G4Material* pyrex = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pyrex_Glass");

    G4LogicalVolume* separation_wall_logic =
      new G4LogicalVolume(separation_wall_solid, pyrex, "SEPARATION_WALL");

    new G4PVPlacement(0, G4ThreeVector(-sep_wall_x_pos, sep_wall_y_pos, 0), separation_wall_logic,
                        "SEPARATION_WALL1", LXe_logic_, false, 1, true);

    new G4PVPlacement(0, G4ThreeVector(sep_wall_x_pos, sep_wall_y_pos, 0), separation_wall_logic,
                        "SEPARATION_WALL2", LXe_logic_, false, 2, true);


    //PYREX WALLS SURROUNDING THE SIPM DICE BOARDS
    G4double wall_width = active_depth_ + tile_thickn_;
    G4double horiz_wall_length = full_row_size_ + 2.*dist_dice_walls_;
    G4double wall_x_pos = ih_x_size_/2. + dist_ihat_wall_ + wall_thickness_ + active_depth_ + tile_thickn_ - wall_width/2.;
    G4double wall_y_pos = full_col_size_/2. + dist_dice_walls_ + wall_thickness_/2.;

    G4Box* surrounding_sensors_horiz_wall_solid =
      new G4Box("SURROUNDING_SENSORS_HORIZ_WALL", wall_width/2., wall_thickness_/2., horiz_wall_length/2.);

    G4LogicalVolume* surrounding_sensors_horiz_wall_logic =
      new G4LogicalVolume(surrounding_sensors_horiz_wall_solid, pyrex, "SURROUNDING_SENSORS_HORIZ_WALL");

    new G4PVPlacement(0, G4ThreeVector(-wall_x_pos, wall_y_pos, 0), surrounding_sensors_horiz_wall_logic,
                       "SURROUNDING_SENSORS_HORIZ_WALL1", LXe_logic_, false, 1, true);

    new G4PVPlacement(0, G4ThreeVector(-wall_x_pos, -wall_y_pos, 0), surrounding_sensors_horiz_wall_logic,
                        "SURROUNDING_SENSORS_HORIZ_WALL2", LXe_logic_, false, 2, true);

    new G4PVPlacement(0, G4ThreeVector(wall_x_pos, wall_y_pos, 0), surrounding_sensors_horiz_wall_logic,
                      "SURROUNDING_SENSORS_HORIZ_WALL3", LXe_logic_, false, 3, true);

    new G4PVPlacement(0, G4ThreeVector(wall_x_pos, -wall_y_pos, 0), surrounding_sensors_horiz_wall_logic,
                      "SURROUNDING_SENSORS_HORIZ_WALL4", LXe_logic_, false, 4, true);


    G4double vert_wall_length = full_col_size_ + 2.*dist_dice_walls_;
    G4double wall_z_pos = full_row_size_/2. + dist_dice_walls_ + wall_thickness_/2.;
    G4Box* surrounding_sensors_vert_wall_solid =
      new G4Box("SURROUNDING_SENSORS_VERT_WALL", wall_width/2., vert_wall_length/2., wall_thickness_/2.);

    G4LogicalVolume* surrounding_sensors_vert_wall_logic =
      new G4LogicalVolume(surrounding_sensors_vert_wall_solid, pyrex, "SURROUNDING_SENSORS_VERT_WALL");

    new G4PVPlacement(0, G4ThreeVector(-wall_x_pos, 0, -wall_z_pos), surrounding_sensors_vert_wall_logic,
                      "SURROUNDING_SENSORS_VERT_WALL1", LXe_logic_, false, 1, true);

    new G4PVPlacement(0, G4ThreeVector(-wall_x_pos, 0, wall_z_pos), surrounding_sensors_vert_wall_logic,
                      "SURROUNDING_SENSORS_VERT_WALL2", LXe_logic_, false, 2, true);

    new G4PVPlacement(0, G4ThreeVector(wall_x_pos, 0, -wall_z_pos), surrounding_sensors_vert_wall_logic,
                      "SURROUNDING_SENSORS_VERT_WALL3", LXe_logic_, false, 3, true);

    new G4PVPlacement(0, G4ThreeVector(wall_x_pos, 0, wall_z_pos), surrounding_sensors_vert_wall_logic,
                      "SURROUNDING_SENSORS_VERT_WALL4", LXe_logic_, false, 4, true);


    // Optical surface for the walls
    G4OpticalSurface* wall_opsur = new G4OpticalSurface("OP_WALL");
    wall_opsur->SetType(dielectric_metal);
    wall_opsur->SetModel(unified);
    wall_opsur->SetFinish(ground);
    wall_opsur->SetSigmaAlpha(0.1);
    wall_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::ReflectantSurface(reflectivity_));
    new G4LogicalSkinSurface("OP_WALL", separation_wall_logic, wall_opsur);
    new G4LogicalSkinSurface("OP_WALL_H", surrounding_sensors_horiz_wall_logic, wall_opsur);
    new G4LogicalSkinSurface("OP_WALL_V", surrounding_sensors_vert_wall_logic, wall_opsur);


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
      G4VisAttributes active_col = nexus::Blue();
      active_logic->SetVisAttributes(active_col);
      G4VisAttributes w_col = nexus::Red();
      separation_wall_logic->SetVisAttributes(w_col);
    }
    else {
      box_logic->SetVisAttributes(G4VisAttributes::Invisible);
      LXe_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  void PetBox::BuildSensors()
  {
    G4LogicalVolume* tile_logic = tile_->GetLogicalVolume();
    G4double x_pos = ih_x_size_/2. + dist_ihat_wall_ + wall_thickness_ + active_depth_ + tile_thickn_/2.;

    G4RotationMatrix rot;
    rot.rotateY(-pi/2.);
    G4ThreeVector position;
    G4String vol_name;

    G4int copy_no = 1;

    for (G4int i=0; i<n_tile_rows_; i++) {
      G4double y_pos = full_col_size_/2. - tile_->GetDimensions().y()/2. - i*tile_->GetDimensions().y();
      for (G4int j=0; j<n_tile_columns_; j++) {
        G4double z_pos = -full_row_size_/2. + tile_->GetDimensions().x()/2. + j*tile_->GetDimensions().x();
        position = G4ThreeVector(x_pos, y_pos, z_pos);
        vol_name = "TILE_" + std::to_string(copy_no);
        new G4PVPlacement(G4Transform3D(rot, position), tile_logic,
                          vol_name, LXe_logic_, false, copy_no, true);
        copy_no += 1;
      }
    }

    rot.rotateY(pi);

    for (G4int i=0; i<n_tile_rows_; i++) {
      G4double y_pos = full_col_size_/2. - tile_->GetDimensions().y()/2. - i*tile_->GetDimensions().y();
      for (G4int j=0; j<n_tile_columns_; j++) {
        G4double z_pos = full_row_size_/2. - tile_->GetDimensions().x()/2. - j*tile_->GetDimensions().x();
        position = G4ThreeVector(-x_pos, y_pos, z_pos);
        vol_name = "TILE_" + std::to_string(copy_no);
        new G4PVPlacement(G4Transform3D(rot, position), tile_logic,
                          vol_name, LXe_logic_, false, copy_no, true);
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
