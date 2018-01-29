// ----------------------------------------------------------------------------
//  $Id: PetitCell.cc  $
//
// ----------------------------------------------------------------------------

#include "PetitCell.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "PetitKDB.h"
#include "PetitPlainDice.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
#include "Visibilities.h"

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

  PetitCell::PetitCell():
    BaseGeometry(),
    // Detector dimensions
    //    vacuum_thickn_(1.*mm),
    //   outer_wall_thickn_(3.*mm),
    det_in_diam_(8.*cm),
    det_length_(30.*cm),
    det_thickness_(1.*mm),
    max_step_size_(1.*mm),
    active_z_dim_(50.*mm),
    active_xy_dim_(50.*mm),
    dice_offset_(0.5*mm)
  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetitCell/",
                                  "Control commands of geometry PetitCell.");

    db_ = new PetitKDB();
    pdb_ = new PetitPlainDice();

  }


  PetitCell::~PetitCell()
  {
  }


  void PetitCell::Construct()
  {
    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = 1.*m;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

    lab_logic_ =
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic_);

    lXe_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    lXe_->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());

    db_->Construct();
    db_thickness_ = db_->GetDimensions().z();
    db_lateral_dim_ = db_->GetDimensions().x();

    pdb_->Construct();
    pdb_thickness_ = pdb_->GetDimensions().z();

    BuildDetector();
    BuildLXe() ;
    BuildActive();
    BuildSiPMPlane();
  }

  void PetitCell::BuildDetector()
  {
    G4double det_out_diam = det_in_diam_ + 2.*det_thickness_;
    G4Tubs* det_solid =
      new G4Tubs("CF100", 0., det_out_diam/2., det_length_/2., 0, twopi);
    G4Material* steel = MaterialsList::Steel();

    det_logic_ = new G4LogicalVolume(det_solid, steel, "CF100");
    //det_logic_->SetVisAttributes(G4VisAttributes::Invisible);

    G4RotationMatrix rot;
    //    rot.rotateX(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0.,0.,0.)), det_logic_,
    		      "CF100", lab_logic_, false, 0, true);
  }

 void PetitCell::BuildLXe()
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

  void PetitCell::BuildActive()
  {
    G4Box* active_solid =
      new G4Box("ACTIVE", active_xy_dim_/2., active_xy_dim_/2., active_z_dim_/2.);

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, lXe_, "ACTIVE");
    //active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes red_color = nexus::Red();
    red_color.SetForceSolid(true);
    active_logic->SetVisAttributes(red_color);

    G4double pos_active = det_length_/2. - pdb_thickness_/2. - dice_offset_ - db_lateral_dim_/2.;
    G4cout << "Center of active volume in LXe = "<< pos_active << G4endl;
    new G4PVPlacement(0, G4ThreeVector(0., 0, pos_active), active_logic,
		      "ACTIVE", lXe_logic_, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive active
    IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

    active_gen_ = new BoxPointSampler(active_xy_dim_, active_xy_dim_, active_z_dim_, 0.,
				      G4ThreeVector(0., 0., pos_active) ,0);


  }

  void PetitCell::BuildSiPMPlane()
  {

    G4LogicalVolume* db_logic = db_->GetLogicalVolume();
    G4LogicalVolume* pdb_logic = pdb_->GetLogicalVolume();
    
    G4double displ_axial = db_lateral_dim_/2. + dice_offset_;
    G4double displ_length_outer = det_length_/2. - pdb_thickness_/2.;
    G4double displ_length_inner =
      det_length_/2. - pdb_thickness_/2. - dice_offset_ - db_lateral_dim_ - dice_offset_;

    new G4PVPlacement(0, G4ThreeVector(0.,0., displ_length_inner), pdb_logic,
     		      "LXE_DICE", lXe_logic_, false, 0, true);

    new G4PVPlacement(0, G4ThreeVector(0.,0., displ_length_outer), pdb_logic,
     		      "LXE_DICE", lXe_logic_, false, 1, true);

    //  G4cout << " LXe outer box starts at " << displ  - db_zsize/2. << "and ends at " << displ + db_zsize/2. << G4endl;

    G4RotationMatrix rot;

    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot,
                                    G4ThreeVector(-displ_axial, 0., displ_length_outer - dice_offset_ - db_lateral_dim_/2.)),
                      pdb_logic, "LXE_DICE", lXe_logic_, false, 2, true);

    //    rot.rotateY(-pi/2.);
    new G4PVPlacement(G4Transform3D(rot,
                                    G4ThreeVector(displ_axial, 0., displ_length_outer - dice_offset_ - db_lateral_dim_/2.)),
                      pdb_logic, "LXE_DICE", lXe_logic_, false, 3, true);

    rot.rotateZ(pi/2.);
    new G4PVPlacement(G4Transform3D(rot,
                                    G4ThreeVector(0., -displ_axial, displ_length_outer - dice_offset_ - db_lateral_dim_/2.)),
                      db_logic, "LXE_DICE", lXe_logic_, false, 0, true);

    rot.rotateZ(pi);
    new G4PVPlacement(G4Transform3D(rot,
                                    G4ThreeVector(0., displ_axial, displ_length_outer - dice_offset_ - db_lateral_dim_/2.)),
                      db_logic, "LXE_DICE", lXe_logic_, false, 1, true);

  }

  G4ThreeVector PetitCell::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    if (region == "ACTIVE") {
      vertex = active_gen_->GenerateVertex("INSIDE");
    } else if (region == "CENTER") {
      vertex = G4ThreeVector(0., 5.*mm, -5.*mm);
    }

    return vertex;
  }

} //end namespace nexus
