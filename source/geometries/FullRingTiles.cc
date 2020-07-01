// ----------------------------------------------------------------------------
// nexus | FullRingTiles.cc
//
// This class implements the geometry of a cylindric ring of LXe,
// of configurable length and diameter, instrumented with specific tiles.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "FullRingTiles.h"
#include "SiPMpetFBK.h"
#include "Tile.h"
#include "CylinderPointSampler.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4LogicalVolume.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4OpticalSurface.hh>

#include <stdexcept>

namespace nexus {

  FullRingTiles::FullRingTiles():
    BaseGeometry(),
    // Detector dimensions
    lat_dimension_cell_(48.*mm), // 52.*mm for quads
    //sipm_pitch_(4.*mm),
    n_tile_rows_(2),
    instr_faces_(1),
    kapton_thickn_(0.3*mm),
    depth_(3.*cm),
    inner_radius_(165*cm),
    cryo_width_(12.*cm),
    cryo_thickn_(1.*mm),
    max_step_size_(1.*mm)
  {
     // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/FullRingTiles/",
                                  "Control commands of geometry FullRingTiles.");
    
    G4GenericMessenger::Command& depth_cmd =
      msg_->DeclareProperty("depth", depth_, "Dimension in DOI");
    depth_cmd.SetUnitCategory("Length");
    depth_cmd.SetParameterName("depth", false);
    depth_cmd.SetRange("depth>0.");

    G4GenericMessenger::Command& inner_r_cmd =
      msg_->DeclareProperty("inner_radius", inner_radius_, "Inner radius of ring");
    inner_r_cmd.SetUnitCategory("Length");
    inner_r_cmd.SetParameterName("inner_radius", false);
    inner_r_cmd.SetRange("inner_radius>0.");

    msg_->DeclareProperty("tile_rows", n_tile_rows_, "Number of tile rows");
    msg_->DeclareProperty("instrumented_faces", instr_faces_, "Number of instrumented faces");

    tile_ = new Tile();
   
    phantom_diam_ = 12.*cm;
    phantom_length_ = 10.*cm;
    
    cylindric_gen_ = 
      new CylinderPointSampler(0., phantom_length_, phantom_diam_/2., 0., G4ThreeVector (0., 0., 0.));
    
  }

  FullRingTiles::~FullRingTiles()
  {

  }

  void FullRingTiles::Construct()
  { 
    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = 1.*m;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
    
    lab_logic_ = 
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic_);

    tile_->Construct();
    tile_logic_ = tile_->GetLogicalVolume();
    tile_dim_ = tile_->GetDimensions();
    
    lat_dimension_cell_ = tile_dim_.y() *  n_tile_rows_;
    G4cout << "Lateral dimensions (mm) = " << lat_dimension_cell_/mm << G4endl;


    external_radius_ = inner_radius_ + depth_;
    G4cout << "Radial dimensions (mm): "<< inner_radius_/mm << ", " << external_radius_/mm << G4endl;
    BuildCryostat();
    BuildSensors();


  }

  void FullRingTiles::BuildCryostat()
  {
    const G4double space_for_elec = 2. * cm;
    const G4double int_radius_cryo = inner_radius_ - cryo_thickn_ - space_for_elec;
    const G4double ext_radius_cryo = external_radius_ + cryo_thickn_ + space_for_elec;
   

    G4Tubs* cryostat_solid =
      new G4Tubs("CRYOSTAT", int_radius_cryo, ext_radius_cryo, cryo_width_/2., 0, twopi);
    G4Material* steel = MaterialsList::Steel();
    G4LogicalVolume* cryostat_logic =
      new G4LogicalVolume(cryostat_solid, steel, "CRYOSTAT");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), cryostat_logic,
		      "CRYOSTAT", lab_logic_, false, 0, true);


    G4double ext_offset = 4. * mm;
    G4Tubs* LXe_solid =
      new G4Tubs("LXE", inner_radius_ - kapton_thickn_, external_radius_ + ext_offset + kapton_thickn_,
                 (lat_dimension_cell_ + 2.*kapton_thickn_)/2., 0, twopi);
    G4Material* LXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    LXe->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    LXe_logic_ =
      new G4LogicalVolume(LXe_solid, LXe, "LXE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), LXe_logic_,
		      "LXE", cryostat_logic, false, 0, true);

    G4Tubs* active_solid =
      new G4Tubs("ACTIVE", inner_radius_, external_radius_ + ext_offset,
                 lat_dimension_cell_/2., 0, twopi);
    active_logic_ =
      new G4LogicalVolume(active_solid, LXe, "ACTIVE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), active_logic_,
		      "ACTIVE", LXe_logic_, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive det
    IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE");
    active_logic_->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    active_logic_->SetUserLimits(new G4UserLimits(max_step_size_));

    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
        
    G4Tubs* kapton_int_solid =
      new G4Tubs("KAPTON", inner_radius_ - kapton_thickn_, inner_radius_,
                 lat_dimension_cell_/2., 0, twopi);    
    G4LogicalVolume* kapton_int_logic =
      new G4LogicalVolume(kapton_int_solid, kapton, "KAPTON");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), kapton_int_logic,
		      "KAPTON", LXe_logic_, false, 0, true);

    G4Tubs* kapton_ext_solid =
      new G4Tubs("KAPTON", external_radius_ + ext_offset, external_radius_ + ext_offset + kapton_thickn_,
                 lat_dimension_cell_/2., 0, twopi);    
    G4LogicalVolume* kapton_ext_logic =
      new G4LogicalVolume(kapton_ext_solid, kapton, "KAPTON");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), kapton_ext_logic,
    		      "KAPTON", LXe_logic_, false, 0, true);

    G4Tubs* kapton_lat_solid =
      new G4Tubs("KAPTON", inner_radius_ - kapton_thickn_, external_radius_ + ext_offset + kapton_thickn_,
                 kapton_thickn_/2., 0, twopi);
    G4LogicalVolume* kapton_lat_logic =
      new G4LogicalVolume(kapton_lat_solid, kapton, "KAPTON");
    G4double z_pos = lat_dimension_cell_/2. + kapton_thickn_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., z_pos), kapton_lat_logic,
    		      "KAPTON", LXe_logic_, false, 0, true);
    new G4PVPlacement(0, G4ThreeVector(0., 0., -z_pos), kapton_lat_logic,
    		      "KAPTON", LXe_logic_, false, 1, true);

    // OPTICAL SURFACE FOR REFLECTION
    G4OpticalSurface* db_opsur = new G4OpticalSurface("BORDER");
    db_opsur->SetType(dielectric_metal);
    db_opsur->SetModel(unified);
    db_opsur->SetFinish(ground);
    db_opsur->SetSigmaAlpha(0.1);
    db_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::ReflectantSurface(0.));
    new G4LogicalSkinSurface("BORDER", kapton_lat_logic, db_opsur);
    new G4LogicalSkinSurface("BORDER", kapton_int_logic, db_opsur);
    new G4LogicalSkinSurface("BORDER", kapton_ext_logic, db_opsur);

    // G4cout << (external_radius_  - kapton_thickn_) / cm << G4endl;

    G4VisAttributes kapton_col = nexus::CopperBrown();
    //kapton_col.SetForceSolid(true);
    kapton_int_logic->SetVisAttributes(kapton_col);
    kapton_ext_logic->SetVisAttributes(kapton_col);
    kapton_lat_logic->SetVisAttributes(kapton_col);
    // G4VisAttributes active_col = nexus::Blue();
    // active_col.SetForceSolid(true);
    // active_logic->SetVisAttributes(active_col);

  }

  void FullRingTiles::BuildSensors()
  {
    G4double batch_sep = 1. * mm;
    G4double tile_sep = 0.050 * mm;
    G4double block_size = 2 * tile_dim_.x() + tile_sep + batch_sep;
    G4int n_batches_per_row = 2 * pi * external_radius_ / block_size;
    G4cout << "Number of SiPMs: " <<  n_batches_per_row * 2 * n_tile_rows_ * 32 << G4endl;
    G4double step = 2.*pi/n_batches_per_row;

    // The external radius distance must be at the level of the SiPM surfaces
    G4double radius = external_radius_ + tile_dim_.z()/2. - 1.2 * mm;
    
    G4RotationMatrix rot;
    rot.rotateX(pi/2.);

    G4int copy_no = 1;
    //
    for (G4int j=0; j<n_tile_rows_; j++) {
      // The first must be positioned outside the loop
      if (j!=0) rot.rotateZ(step);
      G4double z_pos = -lat_dimension_cell_/2. + (j + 1./2.) * tile_dim_.y();
      G4double x_pos = tile_sep/2. + tile_dim_.x()/2;
      G4double y_pos = radius;
      G4ThreeVector position(x_pos, y_pos, z_pos);
      G4String vol_name = "TILE_" + std::to_string(copy_no);
      new G4PVPlacement(G4Transform3D(rot, position), tile_logic_,
     			vol_name, active_logic_, false, copy_no, false);
      copy_no += 1;
      x_pos = - tile_sep/2. - tile_dim_.x()/2;
      position.setX(x_pos);
      vol_name = "TILE_" + std::to_string(copy_no);
      new G4PVPlacement(G4Transform3D(rot, position), tile_logic_,
      			vol_name, active_logic_, false, copy_no, false);
      copy_no += 1;

      for (G4int i=1; i<n_batches_per_row; ++i) {
         G4double angle = i*step;
         rot.rotateZ(step);
	 x_pos = -radius*sin(angle) + (tile_dim_.x() + tile_sep)/2 * cos(angle);
	 y_pos = radius*cos(angle) + (tile_dim_.x() + tile_sep)/2 * sin(angle);
         position.setX(x_pos);
         position.setY(y_pos);
         vol_name = "TILE_" + std::to_string(copy_no);
         new G4PVPlacement(G4Transform3D(rot, position), tile_logic_,
                           vol_name, active_logic_, false, copy_no, false);
	 copy_no += 1;
	 x_pos = -radius*sin(angle) - (tile_dim_.x() + tile_sep)/2 * cos(angle);
	 y_pos = radius*cos(angle) - (tile_dim_.x() + tile_sep)/2 * sin(angle);
         position.setX(x_pos);
         position.setY(y_pos);
         vol_name = "TILE_" + std::to_string(copy_no);
         new G4PVPlacement(G4Transform3D(rot, position), tile_logic_,
                           vol_name, active_logic_, false, copy_no, false);
	 copy_no += 1;
       }
    }

  }


 void FullRingTiles::BuildPhantom()
  {  
    G4Tubs* phantom_solid = new G4Tubs("PHANTOM",  0., phantom_diam_/2., 
                 phantom_length_/2., 0, twopi);
    G4LogicalVolume* phantom_logic =  
      new G4LogicalVolume(phantom_solid, MaterialsList::PEEK(), "PHANTOM");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), phantom_logic,
		       "PAHNTOM", lab_logic_, false, 0, true);
  }
  

  G4ThreeVector FullRingTiles::GenerateVertex(const G4String& region) const
  {  
    G4ThreeVector vertex(0.,0.,0.);

    if (region == "CENTER") {
      return vertex;
    } else if (region == "PHANTOM") {
      vertex = cylindric_gen_->GenerateVertex("BODY_VOL");
    } else {
      G4Exception("[FullRingTiles]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");     
    }
    
    return vertex;
  }

}
