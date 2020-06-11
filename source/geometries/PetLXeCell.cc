// ----------------------------------------------------------------------------
// nexus | PetLXeCell.cc
//
// Basic cell filled with LXe.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PetLXeCell.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "PetPlainDice.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
//#include "SiPMpetBlue.h"
#include "SiPMpetVUV.h"
#include "Visibilities.h"
//#include "SiPMpetBlue.h"
//#include "SiPMpetTPB.h"
//#include "SiPMpetPlots.h"
//#include "Photocathode.h"
//#include "PetKDBFixedPitch.h"
#include <G4GenericMessenger.hh>
#include <G4Box.hh>
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

  PetLXeCell::PetLXeCell():
    BaseGeometry(),

    // Detector dimensions
    //    vacuum_thickn_(1.*mm),
    //   outer_wall_thickn_(3.*mm),
    det_thickness_(1.*mm),
    max_step_size_(1.*mm),
    //  det_size_(20.*cm),
    z_size_(5.*cm),
    xy_size_ (2.5*cm),
    phys_(true),
    z_LXe_(10.*cm)

  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetLXeCell/",
                                  "Control commands of geometry PetalX.");

    // z size
     G4GenericMessenger::Command& zsize_cmd =
       msg_->DeclareProperty("z_size", z_size_, "z dimension");
     zsize_cmd.SetUnitCategory("Length");
     zsize_cmd.SetParameterName("z_size", false);
     zsize_cmd.SetRange("z_size>0.");

      // xy size
     G4GenericMessenger::Command& xysize_cmd =
       msg_->DeclareProperty("xy_size", xy_size_, "xy dimension");
     xysize_cmd.SetUnitCategory("Length");
     xysize_cmd.SetParameterName("xy_size", false);
     xysize_cmd.SetRange("xy_size>0.");

     //Are we using physical opt properties?
     msg_->DeclareProperty("physics", phys_, "physical optical properties");

    // Maximum Step Size
    G4GenericMessenger::Command& step_cmd =
      msg_->DeclareProperty("max_step_size", max_step_size_,
                            "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");

    msg_->DeclareProperty("rows", rows_, "Number of rows in SiPMs");
    msg_->DeclareProperty("columns", columns_, "Number of columns in SiPMs");

    // G4GenericMessenger::Command& pitch_cmd =
    //    msg_->DeclareProperty("sipm_pitch", sipm_pitch_, "Distance between SiPMs");
    //  pitch_cmd.SetUnitCategory("Length");
    //  pitch_cmd.SetParameterName("sipm_pitch", false);
    //  pitch_cmd.SetRange("sipm_pitch>0.");

    //   db_ = new PetKDBFixedPitch();
    pdb_ = new PetPlainDice();

    //G4cout << "VUV sensitive SiPM are being instantiated" << G4endl;

    sipm_ = new SiPMpetVUV;

  }


  PetLXeCell::~PetLXeCell()
  {
    delete active_gen_;
    delete surf_gen_;
  }


  void PetLXeCell::Construct()
  {

    pdb_->SetSize(z_size_, xy_size_);
    pdb_->Construct();
    pdb_z_ = pdb_->GetDimensions().z();

    sipm_->Construct();

    // Vertex Generators
    active_gen_ = new BoxPointSampler(xy_size_, xy_size_, z_size_, 0.,
				      G4ThreeVector(0.,0.,0.) ,0);

    double zpos_generation =
      z_size_/2. + sipm_->GetDimensions().z() + det_thickness_ + 1.*mm;
    surf_gen_ =  new BoxPointSampler(xy_size_, xy_size_, 0., 0.,
				     G4ThreeVector(0.,0., -zpos_generation) ,0);


    lXe_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    if (phys_) {
      G4cout << "LXe used with optical properties" << G4endl;
      lXe_->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
      //lXe_->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe_window());
    } else {
      G4cout << "LXe used with constant refraction index = 1.7" << G4endl;
      lXe_->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe_nconst());
    }

    //    BuildDetector();
    BuildLXe() ;
    BuildActive();
    BuildSiPMPlane();
    // BuildSiPMChessPlane();
  }


  // void PetLXeCell::BuildDetector()
  // {
  //   G4double det_size = xy_size_ + 2.*pdb_z_ + 2.*det_thickness_;
  //   G4double det_size_z =
  //     z_size_+ 2.*sipm_->GetDimensions().z() + 2.*det_thickness_;
  //   G4Box* det_solid =
  //     new G4Box("WALL", det_size/2., det_size/2., det_size_z/2.);
  //   G4Material* steel = MaterialsList::Steel();

  //   det_logic_ = new G4LogicalVolume(det_solid, steel, "WALL");
  //   det_logic_->SetVisAttributes(G4VisAttributes::Invisible);
  //   this->SetLogicalVolume(det_logic_);

  //   // new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), det_logic_,
  //   //     	      "WALL", lab_logic_, false, 0, true);

  //   SetDimensions(G4ThreeVector(det_size/2., det_size/2., det_size_z/2.));
  // }

 void PetLXeCell::BuildLXe()
  {
    G4double lXe_size_xy = xy_size_ + 2.*pdb_z_ + 30.*cm;
    lXe_size_z_ = z_size_ + 2.*pdb_z_ + z_LXe_;
    G4Box* lXe_solid =
      new G4Box("LXE", lXe_size_xy/2., lXe_size_xy/2., lXe_size_z_/2.);

    lXe_logic_ = new G4LogicalVolume(lXe_solid, lXe_, "LXE");
    this->SetLogicalVolume(lXe_logic_);
    SetDimensions(G4ThreeVector(lXe_size_xy, lXe_size_xy, lXe_size_z_));
    G4VisAttributes col = nexus::Lilla();
    lXe_logic_->SetVisAttributes(col);
    // lXe_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    // new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), lXe_logic_,
    //     	      "LXE", det_logic_, false, 0, true);
  }

  void PetLXeCell::BuildActive()
  {
    G4Box* active_solid =
      new G4Box("ACTIVE", xy_size_/2., xy_size_/2., z_size_/2.);

    active_logic_ =
      new G4LogicalVolume(active_solid, lXe_, "ACTIVE_LXE");
    //active_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes col = nexus::DarkGreen();
    col.SetForceSolid(true);
    active_logic_->SetVisAttributes(col);

    new G4PVPlacement(0, G4ThreeVector(0., 0., lXe_size_z_/2.-pdb_z_-xy_size_/2.), active_logic_,
		      "ACTIVE_LXE", lXe_logic_, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive active
    IonizationSD* ionisd = new IonizationSD("/PETALX/ACTIVE_LXe");
    active_logic_->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    std::cout << "*** Maximum Step Size (mm): " << max_step_size_/mm << std::endl;
    active_logic_->SetUserLimits(new G4UserLimits(max_step_size_));

  }

  void PetLXeCell::BuildSiPMPlane()
  {
    G4cout << "Active size = " << xy_size_ << ", " << z_size_ << G4endl;
    sipm_pitch_ = xy_size_ / rows_;
    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();

    // G4double pos_z = db_z/2. - border+ (sipm_->GetDimensions().z())/2.;
    G4double pos_z =  - z_size_/2. + (sipm_->GetDimensions().z())/2 ;

    G4cout << "Dimensions of SiPMs: " << sipm_->GetDimensions() << G4endl;
    G4cout << "Pos of SiPM in LXe cell = " << pos_z << G4endl;
    G4cout << "SiPM pitch = " << sipm_pitch_ << G4endl;

    G4double offset_x = (sipm_pitch_ - sipm_->GetDimensions().x()) / 2.;
    G4double offset_y = (sipm_pitch_ - sipm_->GetDimensions().y()) / 2.;

    G4int sipm_no = 0;

    for (G4int i=0; i<rows_; i++) {
      G4double pos_y = xy_size_/2. - offset_y - sipm_->GetDimensions().y()/2. - i*sipm_pitch_;
      for (G4int j=0; j<columns_; j++) {
        G4double pos_x = -xy_size_/2 + offset_x + sipm_->GetDimensions().x()/2. + j*sipm_pitch_;
        //       G4cout << pos_x << ", " << pos_y << ", " << pos_z << G4endl;
        new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z),
          sipm_logic, "SIPMpet", active_logic_, false, sipm_no, true);

        // std::pair<int, G4ThreeVector> mypos;
        // mypos.first = sipm_no;
        // mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
        // positions_.push_back(mypos);
        sipm_no++;
      }
    }

    pos_z =  z_size_/2. - (sipm_->GetDimensions().z())/2 ;
    G4RotationMatrix rot;
    rot.rotateY(pi);

    sipm_no = 2000;

    for (G4int i=0; i<rows_; i++) {
      G4double pos_y = xy_size_/2. - offset_y - sipm_->GetDimensions().y()/2. - i*sipm_pitch_;
      for (G4int j=0; j<columns_; j++) {
        G4double pos_x = xy_size_/2 - offset_x - sipm_->GetDimensions().x()/2.- j*sipm_pitch_;
	//	G4cout << pos_x << ", " << pos_y << ", " << pos_z << G4endl;
        new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(pos_x, pos_y, pos_z)),
          sipm_logic, "SIPMpet", active_logic_, false, sipm_no, true);
        // std::pair<int, G4ThreeVector> mypos;
        // mypos.first = sipm_no;
        // mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
        // positions_.push_back(mypos);
        sipm_no++;
      }
    }

    G4LogicalVolume* pdb_logic = pdb_->GetLogicalVolume();

    G4double displ = xy_size_/2. + pdb_z_/2.;
    G4double displz = lXe_size_z_/2. - pdb_z_ - z_size_/2.;

    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(-displ, 0., displz)), pdb_logic,
                      "DICE", lXe_logic_, false, 1, true);
    rot.rotateZ(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., displ, displz)), pdb_logic,
                      "DICE", lXe_logic_, false, 2, true);
    rot.rotateZ(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(displ, 0., displz)), pdb_logic,
                      "DICE", lXe_logic_, false, 3, true);
    rot.rotateZ(pi/2);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., -displ, displz)), pdb_logic,
                      "DICE", lXe_logic_, false, 4, true);


    displz = lXe_size_z_/2. - pdb_z_/2.;
    rot.rotateX(pi/2);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., 0., displz)), pdb_logic,
                      "DICE", lXe_logic_, false, 6, true);
    rot.rotateY(pi);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., 0., displz - pdb_z_/2. - z_size_ - pdb_z_/2.))
                      , pdb_logic, "DICE", lXe_logic_, false, 5, true);



  }

 // void PetLXeCell::BuildSiPMChessPlane()
 //  {

 //    G4int rows = 8;
 //    G4int columns = 8;
 //    G4LogicalVolume* sipmTPB_logic = sipmTPB_->GetLogicalVolume();
 //    G4LogicalVolume* sipmBlue_logic = sipmBlue_->GetLogicalVolume();

 //    // G4double pos_z = db_z/2. - border+ (sipm_->GetDimensions().z())/2.;
 //    G4double pos_z =  -z_size_/2. - (sipmBlue_->GetDimensions().z())/2 ;
 //    G4cout << "Pos of SiPM in LXe dice = " << pos_z << G4endl;

 //    G4double sipm_pitch = sipmBlue_->GetDimensions().x();
 //    G4cout << "SiPM pitch = " << sipm_pitch << G4endl;

 //    G4int sipm_no = 0;

 //    for (G4int i=0; i<rows; i++) {

 //      G4double pos_y = xy_size_/2. - sipmBlue_->GetDimensions().y()/2. - i*sipm_pitch;

 //      for (G4int j=0; j<columns; j++) {

 //        G4double pos_x = -xy_size_/2  + sipmBlue_->GetDimensions().x()/2. +  j*sipm_pitch;

 // 	//	G4cout << pos_x << ", " << pos_y << ", " << pos_z << G4endl;

 // 	if ((i+j) % 2 == 0) {
 // 	  new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z),
 // 			    sipmBlue_logic, "SIPMpetBlue", lXe_logic_, false, sipm_no, true);
 // 	} else {
 // 	  new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z),
 // 			    sipmTPB_logic, "SIPMpet", lXe_logic_, false, sipm_no, true);
 // 	}

 //        // std::pair<int, G4ThreeVector> mypos;
 //        // mypos.first = sipm_no;
 //        // mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
 //        // positions_.push_back(mypos);
 //        sipm_no++;
 //      }
 //    }

 //    pos_z =  z_size_/2. + (sipmBlue_->GetDimensions().z())/2 ;
 //    G4RotationMatrix rot;
 //    rot.rotateY(pi);

 //    sipm_no = 2000;

 //    for (G4int i=0; i<rows; i++) {

 //      G4double pos_y = xy_size_/2. - sipmBlue_->GetDimensions().y()/2.  - i*sipm_pitch;

 //      for (G4int j=0; j<columns; j++) {

 //        G4double pos_x = xy_size_/2 - sipmBlue_->GetDimensions().x()/2.  -  j*sipm_pitch;

 // 	//	G4cout << pos_x << ", " << pos_y << ", " << pos_z << G4endl;

 // 	if ((i+j) % 2 == 0) {
 // 	  new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(pos_x, pos_y, pos_z)),
 // 			    sipmBlue_logic, "SIPMpetBlue", lXe_logic_, false, sipm_no, true);
 // 	} else {
 // 	  new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(pos_x, pos_y, pos_z)),
 // 			    sipmTPB_logic, "SIPMpet", lXe_logic_, false, sipm_no, true);
 // 	}

 //        // std::pair<int, G4ThreeVector> mypos;
 //        // mypos.first = sipm_no;
 //        // mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
 //        // positions_.push_back(mypos);
 //        sipm_no++;
 //      }
 //    }

 //    G4LogicalVolume* pdb_logic = pdb_->GetLogicalVolume();

 //    G4double displ = xy_size_/2. + pdb_z_/2.;
 //    rot.rotateY(pi/2.);
 //    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(-displ, 0., 0.)), pdb_logic,
 //                      "DICE", lXe_logic_, false, 1, true);
 //    rot.rotateZ(pi/2.);
 //    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., displ, 0.)), pdb_logic,
 //                      "DICE", lXe_logic_, false, 2, true);
 //    rot.rotateZ(pi/2.);
 //     new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(displ, 0., 0.)), pdb_logic,
 //                      "DICE", lXe_logic_, false, 3, true);
 //     rot.rotateZ(pi/2);
 //     new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., -displ, 0.)), pdb_logic,
 // 		       "DICE", lXe_logic_, false, 4, true);


 //  }

  G4ThreeVector PetLXeCell::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // ACTIVE
    if (region == "ACTIVE") {
      vertex = active_gen_->GenerateVertex(region);
    } else if (region == "CENTER") {
      vertex = G4ThreeVector(1.5*mm, 1.5*mm, 0.*mm);
    } else if (region == "SURFACE") {
      vertex = surf_gen_->GenerateVertex("Z_SURF");
    }

    return vertex;
  }


} //end namespace nexus
