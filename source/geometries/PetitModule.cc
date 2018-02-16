// ----------------------------------------------------------------------------
//  $Id: PetitModule.cc  $
//  Author:  <romo@ific.uv.es>
//  Created: February 2018
//  Copyright (c) 2015-2017 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PetitModule.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "PetitPlainDice.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
#include "Visibilities.h"
#include "SiPMpet9mm2.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>
#include <G4NistManager.hh>
#include <G4UniformMagField.hh>
#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4RotationMatrix.hh>


namespace nexus {

  PetitModule::PetitModule():
    BaseGeometry(),
    // Detector dimensions
    det_in_diam_(8.*cm),
    det_length_(30.*cm),
    det_thickness_(1.*mm),
    max_step_size_(1.*mm),
    active_z_dim_(50.*mm),
    active_xy_dim_(50.*mm),
    dice_offset_(1.*mm),
    two_instrumented_planes_(0),
    visibility_dices_(1)
  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetitModule/",
                                  "Control commands of geometry Petit.");

    // Diameter of the cylinder
    G4GenericMessenger::Command& det_in_diam_cmd =
    msg_->DeclareProperty("det_in_diam", det_in_diam_, "Diameter of the detector");
    det_in_diam_cmd.SetUnitCategory("Length");
    det_in_diam_cmd.SetParameterName("det_in_diam", false);
    det_in_diam_cmd.SetRange("det_in_diam>0.");

    // Length of the cylinder
    G4GenericMessenger::Command& det_length_cmd =
    msg_->DeclareProperty("det_length", det_length_, "Length of the detector");
    det_length_cmd.SetUnitCategory("Length");
    det_length_cmd.SetParameterName("det_length", false);
    det_length_cmd.SetRange("det_length>0.");

    // Maximum Step Size
    G4GenericMessenger::Command& step_cmd =
    msg_->DeclareProperty("max_step_size", max_step_size_,
                        "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");

    // Number of instrumented faces
    msg_->DeclareProperty("two_instrumented_planes", two_instrumented_planes_,
                "True if the number of instrumented planes is two, false if the number is one");

    // Visibility of the Kapton Dice Boards
    msg_->DeclareProperty("visibility_dices", visibility_dices_,
                          "PET Kapton Dice Boards Visibility");


    // If region is AD_HOC
    G4GenericMessenger::Command&  specific_vertex_X_cmd =
    msg_->DeclareProperty("specific_vertex_X", specific_vertex_X_,
                    "If region is AD_HOC, x coord where particles are generated");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
    msg_->DeclareProperty("specific_vertex_Y", specific_vertex_Y_,
                    "If region is AD_HOC, y coord where particles are generated");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
    msg_->DeclareProperty("specific_vertex_Z", specific_vertex_Z_,
                    "If region is AD_HOC, z coord where particles are generated");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");


    sipm_ = new SiPMpet9mm2();
    pdb_ = new PetitPlainDice(); // Kapton faces
  }

  PetitModule::~PetitModule()
  {
  }

  void PetitModule::Construct()
  {
    G4double det_out_diam = det_in_diam_ + 2.*det_thickness_;
    G4Tubs* det_solid =
    new G4Tubs("CF100", 0., det_out_diam/2., det_length_/2., 0, twopi);
    G4Material* steel = MaterialsList::Steel();

    det_logic_ = new G4LogicalVolume(det_solid, steel, "CF100");
    //det_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes lilla_color = nexus::Lilla(); // Visibilities
    lilla_color.SetForceSolid(false);
    det_logic_->SetVisAttributes(lilla_color);

    this->SetLogicalVolume(det_logic_);

    SetDiameter(det_out_diam);
    SetLength(det_length_);

    lXe_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    lXe_->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());

    sipm_->Construct();

    pdb_->Construct();
    pdb_lateral_dim_ = pdb_->GetDimensions().x();
    pdb_thickness_ = pdb_->GetDimensions().z();

    BuildLXe() ;
    BuildActive();
    BuildSiPMPlane();
  }


  void PetitModule::BuildLXe()
  {
    G4double lXe_diam = det_in_diam_;
    G4double lXe_length = det_length_;
    G4Tubs* lXe_solid =
      new G4Tubs("LXE", 0., lXe_diam/2., lXe_length/2., 0, twopi);

    lXe_logic_ = new G4LogicalVolume(lXe_solid, lXe_, "LXE");
    lXe_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), lXe_logic_,
		      "LXE", det_logic_, false, 0, true);
  }


  void PetitModule::BuildActive()
  {
    G4Box* active_solid =
      new G4Box("ACTIVE", active_xy_dim_/2., active_xy_dim_/2., active_z_dim_/2.);

    active_logic_ = new G4LogicalVolume(active_solid, lXe_, "ACTIVE");
    active_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    // G4VisAttributes red_color = nexus::Red(); // Visibilities
    // red_color.SetForceSolid(true);
    // active_logic_->SetVisAttributes(red_color);

    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), active_logic_,
                      "ACTIVE", lXe_logic_, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive active
    IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE");
    active_logic_->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    active_logic_->SetUserLimits(new G4UserLimits(max_step_size_));

    active_gen_ = new BoxPointSampler(active_xy_dim_, active_xy_dim_, active_z_dim_, 0.,
				      G4ThreeVector(0., 0., 0.) ,0);
  }


  void PetitModule::BuildSiPMPlane()
  {
    G4LogicalVolume* pdb_logic = pdb_->GetLogicalVolume();
    G4double displ = pdb_lateral_dim_/2. + dice_offset_; // pdb_thickness_/2. is included in dice_offset_;


    // THE CELL IS CENTERED IN THE CYLINDER

    // Non instrumented faces:
    new G4PVPlacement(0, G4ThreeVector(0., 0., - displ), pdb_logic,
                    "DICE_BOARD", lXe_logic_, false, 0, true);

    new G4PVPlacement(0, G4ThreeVector(0., 0., displ), pdb_logic,
                    "DICE_BOARD", lXe_logic_, false, 1, true);

    G4RotationMatrix rot;

    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(- displ, 0., 0.)),
                    pdb_logic, "DICE_BOARD", lXe_logic_, false, 2, true);

    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(displ, 0., 0.)),
                    pdb_logic, "DICE_BOARD", lXe_logic_, false, 3, true);


    // Instrumented faces
    // This face can be instrumented or not
    rot.rotateZ(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., displ, 0.)),
                        pdb_logic, "DICE_BOARD", lXe_logic_, false, 4, true);

    // This face is always instrumented
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., - displ, 0.)),
                    pdb_logic, "DICE_BOARD", lXe_logic_, false, 5, true);


    // SETTING VISIBILITIES   //////////
    if (visibility_dices_) {
        G4VisAttributes dices_col = nexus::LightGreen();
        dices_col.SetForceSolid(false);
        pdb_logic->SetVisAttributes(dices_col);
    }
    else {
        pdb_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // SiPM. Watch out! Change of coordinates
    // First instrumented face
    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();
    G4double sipm_xz_dim_ = sipm_->GetDimensions().getX();
    G4double sipm_y_dim_ = sipm_->GetDimensions().getZ();
    G4double pos_y = - displ + pdb_thickness_/2. + sipm_y_dim_/2.;
    G4double offset = sipm_xz_dim_/2.;
    G4double sipm_pitch = sipm_xz_dim_;
    G4int sipm_no = 0;

    const G4int rows = 8;
    const G4int columns = 8;
      
    for (G4int i=0; i<rows; i++) {
        G4double pos_z = - pdb_lateral_dim_/2. + offset + i*sipm_pitch;
        for (G4int j=0; j<columns; j++) {
            G4double pos_x = pdb_lateral_dim_/2. - offset - j*sipm_pitch;
            //G4cout << pos_x << " (mm), " << pos_y << " (mm), " << pos_z << " (mm), " << G4endl;
            new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(pos_x, pos_y, pos_z)),
                              sipm_logic, "SIPMpet", lXe_logic_, false, sipm_no, true);
              sipm_no++;
          }
      }

    // Number of instrumented planes

    if (two_instrumented_planes_) {
        pos_y = - pos_y;
        rot.rotateZ(pi);

        for (G4int i=0; i<rows; i++) {
            G4double pos_z = pdb_lateral_dim_/2. - offset - i*sipm_pitch;
            for (G4int j=0; j<columns; j++) {
                G4double pos_x = pdb_lateral_dim_/2. - offset - j*sipm_pitch;
                //G4cout << pos_x << " (mm), " << pos_y << " (mm), " << pos_z << " (mm), " << G4endl;
                new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(pos_x, pos_y, pos_z)),
                                  sipm_logic, "SIPMpet", lXe_logic_, false, sipm_no, true);
                sipm_no++;
            }
        }
    }
  }


  G4ThreeVector PetitModule::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    if (region == "ACTIVE") {
      vertex = active_gen_->GenerateVertex("INSIDE");
    } else if (region == "CENTER") {
      vertex = G4ThreeVector(0., 0., 0.);
    }

    return vertex;
  }

} //end namespace nexus
