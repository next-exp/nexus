// ----------------------------------------------------------------------------
//  $Id: PuppyCell.cc  $
//
// ----------------------------------------------------------------------------

#include "PuppyCell.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "PuppyKDB.h"
#include "PuppyPlainDice.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"

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

  PuppyCell::PuppyCell():
    BaseGeometry(),
    // Detector dimensions
    //    vacuum_thickn_(1.*mm),
    //   outer_wall_thickn_(3.*mm),
    det_in_diam_(8.*cm),
    det_length_(30.*cm),
    det_thickness_(1.*mm),
    max_step_size_(1.*mm),
    active_z_dim_(5.3*cm), // 52 mm + 0.5 mm each side
    active_xy_dim_(5.3*cm) // 52 mm + 0.5 mm each side
  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PuppyCell/",
                                  "Control commands of geometry PuppyCell.");

    db_ = new PuppyKDB();
    pdb_ = new PuppyPlainDice();

  }


  PuppyCell::~PuppyCell()
  {
  }


  void PuppyCell::Construct()
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
    db_z_ = db_->GetDimensions().z();

    BuildDetector();
    BuildLXe() ;
    BuildActive();
    BuildSiPMPlane();
  }

  void PuppyCell::BuildDetector()
  {
    G4double det_out_diam = det_in_diam_ + 2.*det_thickness_;
    G4Tubs* det_solid =
      new G4Tubs("CF100", 0., det_out_diam/2., det_length_/2., 0, twopi);
    G4Material* steel = MaterialsList::Steel();

    det_logic_ = new G4LogicalVolume(det_solid, steel, "CF100");
    //det_logic_->SetVisAttributes(G4VisAttributes::Invisible);

    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateX(pi/2.);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), det_logic_,
		      "CF100", lab_logic_, false, 0, true);
  }

 void PuppyCell::BuildLXe()
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

  void PuppyCell::BuildActive()
  {
    G4Box* active_solid =
      new G4Box("ACTIVE", active_xy_dim_/2., active_xy_dim_/2., active_z_dim_/2.);

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, lXe_, "ACTIVE");
    //active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes red_color;
    red_color.SetColor(1., 0., 0.);
    red_color.SetForceSolid(true);
    active_logic->SetVisAttributes(red_color);

    G4double pos_y = - det_length_/2. + active_xy_dim_/2./2. ;
    new G4PVPlacement(0, G4ThreeVector(0., pos_y, 0.), active_logic,
		      "ACTIVE", lXe_logic_, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive active
    IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

  }

  void PuppyCell::BuildSiPMPlane()
  {
    pdb_->Construct();

    G4LogicalVolume* db_logic = db_->GetLogicalVolume();
    G4LogicalVolume* pdb_logic = pdb_->GetLogicalVolume();

    G4double db_zsize = db_->GetDimensions().z();
    // G4cout << "dice board x = " << db_xsize << ", y = "
    // 	   << db_ysize << ", z = " <<  db_zsize << std::endl;
    G4double displ_xy = active_xy_dim_/2. + db_zsize/2.;
    G4double displ_z_outer = det_length_/2. - db_zsize/2.;
    G4double displ_z_inner = det_length_/2. - db_zsize - active_z_dim_ - db_zsize/2.;

    new G4PVPlacement(0, G4ThreeVector(0.,0., displ_z_inner), pdb_logic,
     		      "LXE_DICE", lXe_logic_, false, 0, true);

    new G4PVPlacement(0, G4ThreeVector(0.,0., displ_z_outer), pdb_logic,
     		      "LXE_DICE", lXe_logic_, false, 1, true);

    //  G4cout << " LXe outer box starts at " << displ  - db_zsize/2. << "and ends at " << displ + db_zsize/2. << G4endl;

    G4RotationMatrix rot;

    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(-displ_xy, 0., 0.)), pdb_logic,
		      "LXE_DICE", lXe_logic_, false, 2, true);

    //    rot.rotateY(-pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(displ_xy, 0., 0.)), pdb_logic,
		      "LXE_DICE", lXe_logic_, false, 3, true);

    rot.rotateZ(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., displ_xy, 0.)), db_logic,
    		      "LXE_DICE", lXe_logic_, false, 4, true);

    rot.rotateZ(pi);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., -displ_xy, 0.)), db_logic,
    		      "LXE_DICE", lXe_logic_, false, 5, true);

  }

  G4ThreeVector PuppyCell::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    if (region == "OUTSIDE") {
      vertex = G4ThreeVector(0., 0., -20.*cm);
    } else if (region == "CENTER") {
      vertex = G4ThreeVector(0., 5.*mm, -5.*mm);
    }

    return vertex;
  }

} //end namespace nexus
