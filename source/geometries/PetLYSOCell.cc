// ----------------------------------------------------------------------------
// nexus | PetLYSOCell.cc
//
// Basic cell made of LYSO.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PetLYSOCell.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "PetKDBFixedPitch.h"
#include "PetPlainDice.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
#include "SiPMpetVUV.h"
#include "SiPMpetTPB.h"

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

  PetLYSOCell::PetLYSOCell():
    BaseGeometry(),

    // Detector dimensions
    //    vacuum_thickn_(1.*mm),
    //   outer_wall_thickn_(3.*mm),
    det_thickness_(1.*mm),
    //  det_size_(20.*cm),
    active_size_ (2.4*cm),
    //  active_size_(4.8*cm),
    z_size_(5.*cm),
    phys_(true)

  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetLYSOCell/",
                                  "Control commands of geometry Petalo.");

    // z size
     G4GenericMessenger::Command& zsize_cmd =
       msg_->DeclareProperty("z_size", z_size_, "z dimension");
     zsize_cmd.SetUnitCategory("Length");
     zsize_cmd.SetParameterName("z_size", false);
     zsize_cmd.SetRange("z_size>0.");

     //Are we using physical opt properties?
     msg_->DeclareProperty("physics", phys_, "physical optical properties");

    // Maximum Step Size
    G4GenericMessenger::Command& step_cmd =
      msg_->DeclareProperty("max_step_size", max_step_size_,
								  "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");


    pdb_ = new PetPlainDice();

    G4cout << "VUV sensitive SiPM are being instantiated" << G4endl;
    sipm_ = new SiPMpetVUV;
  }


  PetLYSOCell::~PetLYSOCell()
  {
    delete active_gen_;
    delete surf_gen_;
  }


  void PetLYSOCell::Construct()
  {
    // db_->SetXYsize( active_size_);
    // db_->SetPitchSize(3.*mm);
    // db_->Construct();
    // db_z_ = db_->GetDimensions().z();

    pdb_->SetSize(z_size_, active_size_);
    pdb_->Construct();
    pdb_z_ = pdb_->GetDimensions().z();

    sipm_->Construct();

    // Vertex Generators
    active_gen_ = new BoxPointSampler(active_size_, active_size_, z_size_, 0.,
				      G4ThreeVector(0.,0.,0.) ,0);

    double zpos_generation =
      z_size_/2. + sipm_->GetDimensions().z() + det_thickness_ + 1.*mm;
    surf_gen_ =  new BoxPointSampler(active_size_, active_size_, 0., 0.,
				     G4ThreeVector(0.,0., -zpos_generation) ,0);

    // // LAB. This is just a volume of air surrounding the detector
    // G4double lab_size = 1.*m;
    // G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

    // lab_logic_ =
    //   new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    // lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    // this->SetLogicalVolume(lab_logic_);

    lyso_ = MaterialsList::LYSO();
    if (phys_) {
      G4cout << "LYSO used with optical properties" << G4endl;
      lyso_->SetMaterialPropertiesTable(OpticalMaterialProperties::LYSO());
    } else {
      G4cout << "LYSO used with constant refraction index = 1.8" << G4endl;
      lyso_->SetMaterialPropertiesTable(OpticalMaterialProperties::LYSO_nconst());
    }

    BuildDetector();
    BuildLYSO() ;
    BuildActive();
    BuildSiPMPlane();
  }


  void PetLYSOCell::BuildDetector()
  {
    G4double det_size = active_size_ + 2.*pdb_z_ + 2.*det_thickness_;
    G4double det_size2 =
      z_size_+ 2.*sipm_->GetDimensions().z() + 2.*det_thickness_;
    G4Box* det_solid =
      //   new G4Box("WALL", det_size/2., det_size/2., det_size/2.);
      new G4Box("WALL", det_size/2., det_size/2., det_size2/2.);
    G4Material* steel = MaterialsList::Steel();

    det_logic_ = new G4LogicalVolume(det_solid, steel, "WALL");
    det_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(det_logic_);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), det_logic_,
		      "WALL", lab_logic_, false, 0, true);
  }

 void PetLYSOCell::BuildLYSO()
  {
    G4double lyso_size = active_size_ + 2.*pdb_z_ ;
    G4double lyso_size2 = z_size_ + 2.*sipm_->GetDimensions().z();
    G4Box* lyso_solid =
      new G4Box("LYSO", lyso_size/2., lyso_size/2., lyso_size2/2.);

    lyso_logic_ = new G4LogicalVolume(lyso_solid, lyso_, "LYSO");
    lyso_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), lyso_logic_,
 		      "LYSO", det_logic_, false, 0, true);
  }

  void PetLYSOCell::BuildActive()
  {
    G4Box* active_solid =
      new G4Box("ACTIVE_LYSO", active_size_/2., active_size_/2., z_size_/2.);
    // G4Box* active_solid =
    //   new G4Box("ACTIVE", lXe_size/2., lXe_size/2., lXe_size2/2.);

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, lyso_, "ACTIVE_LYSO");
    active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // G4VisAttributes red_color;
    // red_color.SetColor(1., 0., 0.);
    // red_color.SetForceSolid(true);
    // active_logic->SetVisAttributes(red_color);

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic,
		      "ACTIVE_LYSO", lyso_logic_, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive active
    IonizationSD* ionisd = new IonizationSD("/PETALX/ACTIVE_LYSO");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    std::cout << "*** Maximum Step Size (mm): " << max_step_size_/mm << std::endl;
    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

  }

  void PetLYSOCell::BuildSiPMPlane()
  {

    G4int rows = 8;
    G4int columns = 8;
    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();

    // G4double pos_z = db_z/2. - border+ (sipm_->GetDimensions().z())/2.;
    G4double pos_z =  -z_size_/2. - (sipm_->GetDimensions().z())/2 ;
    G4cout << "Pos of SiPM in LYSO dice = " << pos_z << G4endl;

    G4double sipm_pitch = sipm_->GetDimensions().x();
    G4cout << "SiPM pitch = " << sipm_pitch << G4endl;

    G4int sipm_no = 0;

    for (G4int i=0; i<rows; i++) {

      G4double pos_y = active_size_/2. - sipm_->GetDimensions().y()/2. - i*sipm_pitch;

      for (G4int j=0; j<columns; j++) {

        G4double pos_x = -active_size_/2  + sipm_->GetDimensions().x()/2. +  j*sipm_pitch;

	//	G4cout << pos_x << ", " << pos_y << ", " << pos_z << G4endl;

        new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z),
          sipm_logic, "SIPMpet", lyso_logic_, false, sipm_no, true);

        // std::pair<int, G4ThreeVector> mypos;
        // mypos.first = sipm_no;
        // mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
        // positions_.push_back(mypos);
        sipm_no++;
      }
    }

    pos_z =  z_size_/2. + (sipm_->GetDimensions().z())/2 ;
    G4RotationMatrix rot;
    rot.rotateY(pi);

    sipm_no = 2000;

    for (G4int i=0; i<rows; i++) {

      G4double pos_y = active_size_/2. - sipm_->GetDimensions().y()/2.  - i*sipm_pitch;

      for (G4int j=0; j<columns; j++) {

        G4double pos_x = active_size_/2 - sipm_->GetDimensions().x()/2.  -  j*sipm_pitch;

	//	G4cout << pos_x << ", " << pos_y << ", " << pos_z << G4endl;

        new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(pos_x, pos_y, pos_z)),
          sipm_logic, "SIPMpet", lyso_logic_, false, sipm_no, true);

        // std::pair<int, G4ThreeVector> mypos;
        // mypos.first = sipm_no;
        // mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
        // positions_.push_back(mypos);
        sipm_no++;
      }
    }

    G4LogicalVolume* pdb_logic = pdb_->GetLogicalVolume();

    G4double displ = active_size_/2. + pdb_z_/2.;
    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(-displ, 0., 0.)), pdb_logic,
                      "DICE", lyso_logic_, false, 1, true);
    rot.rotateZ(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., displ, 0.)), pdb_logic,
                      "DICE", lyso_logic_, false, 2, true);
    rot.rotateZ(pi/2.);
     new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(displ, 0., 0.)), pdb_logic,
                      "DICE", lyso_logic_, false, 3, true);
     rot.rotateZ(pi/2);
     new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., -displ, 0.)), pdb_logic,
		       "DICE", lyso_logic_, false, 4, true);


  }

  G4ThreeVector PetLYSOCell::GenerateVertex(const G4String& region) const
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
