// ----------------------------------------------------------------------------
//  $Id: FullRingInfinity.cc  $
//
//  Author:  <paolafer@ific.uv.es>   
//  Created: March 2018
//  
//  Copyright (c) 2015 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "FullRingInfinity.h"
#include "SiPMpetFBK.h"
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

  FullRingInfinity::FullRingInfinity():
    BaseGeometry(),
    // Detector dimensions
    lat_dimension_cell_(48.*mm), // 52.*mm for quads
    sipm_pitch_(4.*mm),
    lin_n_sipm_per_cell_(16),
    instr_faces_(2),
    kapton_thickn_(0.3*mm),
    inner_radius_(15.*cm),
    depth_(5.*cm),
    cryo_width_(12.*cm),
    cryo_thickn_(1.*mm),
    max_step_size_(1.*mm)
  {
     // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/FullRingInfinity/",
                                  "Control commands of geometry FullRingInfinity.");
    G4GenericMessenger::Command& depth_cmd =
      msg_->DeclareProperty("depth", depth_, "Dimension in DOI");
    depth_cmd.SetUnitCategory("Length");
    depth_cmd.SetParameterName("depth", false);
    depth_cmd.SetRange("depth>0.");

    G4GenericMessenger::Command& pitch_cmd =
      msg_->DeclareProperty("pitch", sipm_pitch_, "Pitch of SiPMs");
    pitch_cmd.SetUnitCategory("Length");
    pitch_cmd.SetParameterName("pitch", false);
    pitch_cmd.SetRange("pitch>0.");

    G4GenericMessenger::Command& inner_r_cmd =
      msg_->DeclareProperty("inner_radius", inner_radius_, "Inner radius of ring");
    inner_r_cmd.SetUnitCategory("Length");
    inner_r_cmd.SetParameterName("inner_radius", false);
    inner_r_cmd.SetRange("inner_radius>0.");

    msg_->DeclareProperty("sipm_rows", lin_n_sipm_per_cell_, "Number of SiPM rows");
    msg_->DeclareProperty("instrumented_faces", instr_faces_, "Number of instrumented faces");

    sipm_ = new SiPMpetFBK();
   
    phantom_diam_ = 12.*cm;
    phantom_length_ = 10.*cm;
    
    cylindric_gen_ = 
      new CylinderPointSampler(0., phantom_length_, phantom_diam_/2., 0., G4ThreeVector (0., 0., 0.));
    
  }

  FullRingInfinity::~FullRingInfinity()
  {

  }

  void FullRingInfinity::Construct()
  { 
    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = 1.*m;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
    
    lab_logic_ = 
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic_);

    lat_dimension_cell_ = sipm_pitch_ *  lin_n_sipm_per_cell_;
    G4cout << "Lateral dimensions (mm) = " << lat_dimension_cell_/mm << G4endl;


     external_radius_ = inner_radius_ + depth_;
     G4cout << "Radial dimensions (mm): "<< inner_radius_/mm << ", " << external_radius_/mm << G4endl;
     BuildCryostat();
     BuildSensors();


  }

  void FullRingInfinity::BuildCryostat()
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


    G4double ext_offset = 0. * mm;
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
    kapton_col.SetForceSolid(true);
    kapton_int_logic->SetVisAttributes(kapton_col);
    kapton_ext_logic->SetVisAttributes(kapton_col);
    kapton_lat_logic->SetVisAttributes(kapton_col);
    // G4VisAttributes active_col = nexus::Blue();
    // active_col.SetForceSolid(true);
    // active_logic->SetVisAttributes(active_col);

  }

  void FullRingInfinity::BuildSensors()
  {
    sipm_->Construct();

    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();
    G4ThreeVector sipm_dim = sipm_->GetDimensions();
    //G4double sipm_pitch = sipm_dim.x() + 1. * mm;

    G4int n_sipm_int = 2*pi*inner_radius_/sipm_pitch_;
    if (instr_faces_ == 2) {
      G4cout << "Number of sipms in inner face: " <<  n_sipm_int *  lin_n_sipm_per_cell_<< G4endl;
    }
    G4double step = 2.*pi/n_sipm_int;
    G4double radius = inner_radius_ + sipm_dim.z()/2.;

    G4RotationMatrix rot;
    rot.rotateX(-pi/2.);

    G4int copy_no = 999;
    for (G4int j=0; j<lin_n_sipm_per_cell_; j++) {
      // The first must be positioned outside the loop
      if (j!=0) rot.rotateZ(step);
      G4double z_dimension = -lat_dimension_cell_/2. + (j + 1./2.) * sipm_pitch_;
      G4ThreeVector position(0., radius, z_dimension);
      copy_no += 1;
      G4String vol_name = "SIPM_" + std::to_string(copy_no);
      if (instr_faces_ == 2) {
        new G4PVPlacement(G4Transform3D(rot, position), sipm_logic,
                          vol_name, active_logic_, false, copy_no, false);
      }

      for (G4int i=2; i<=n_sipm_int; ++i) {
        G4double angle = (i-1)*step;
        rot.rotateZ(step);
        position.setX(-radius*sin(angle));
        position.setY(radius*cos(angle));
        copy_no += 1;
        vol_name = "SIPM_" + std::to_string(copy_no);
        if (instr_faces_ == 2) {
          new G4PVPlacement(G4Transform3D(rot, position), sipm_logic,
                            vol_name, active_logic_, false, copy_no, false);
        }
      }
    }

    //G4double sipm_pitch_ext = sipm_dim.x() + 0.5 * mm;
    G4double offset = 0.1 * mm;
    G4int n_sipm_ext = 2*pi*external_radius_/sipm_pitch_;
    G4cout << "Number of sipms in external face: " <<  n_sipm_ext * lin_n_sipm_per_cell_ << G4endl;
    step = 2.*pi/n_sipm_ext;
    radius = external_radius_ - sipm_dim.z()/2. - offset;

    rot.rotateZ(step);
    rot.rotateX(pi);

    //copy_no = 2000;
    for (G4int j=0; j<lin_n_sipm_per_cell_; j++) {
      // The first must be positioned outside the loop
      if (j!=0) rot.rotateZ(step);
      G4double z_pos = -lat_dimension_cell_/2. + (j + 1./2.) * sipm_pitch_;
      G4ThreeVector position(0., radius, z_pos);
      copy_no = copy_no + 1;
      G4String vol_name = "SIPM_" + std::to_string(copy_no);
      new G4PVPlacement(G4Transform3D(rot, position), sipm_logic,
                        vol_name, active_logic_, false, copy_no, true);

      for (G4int i=2; i<=n_sipm_ext; ++i) {
        G4double angle = (i-1)*step;
        rot.rotateZ(step);
        position.setX(-radius*sin(angle));
        position.setY(radius*cos(angle));
        copy_no = copy_no + 1;
        vol_name = "SIPM_" + std::to_string(copy_no);
        new G4PVPlacement(G4Transform3D(rot, position), sipm_logic,
                          vol_name, active_logic_, false, copy_no, true);
      }
    }

  }


 void FullRingInfinity::BuildPhantom()
  {
     

    G4Tubs* phantom_solid = new G4Tubs("PHANTOM",  0., phantom_diam_/2., 
                 phantom_length_/2., 0, twopi);
    G4LogicalVolume* phantom_logic =  
      new G4LogicalVolume(phantom_solid, MaterialsList::PEEK(), "PHANTOM");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), phantom_logic,
		       "PAHNTOM", lab_logic_, false, 0, true);   

 
  }
  

  G4ThreeVector FullRingInfinity::GenerateVertex(const G4String& region) const
  {
   
    G4ThreeVector vertex(0.,0.,0.);

    if (region == "CENTER") {
      vertex = vertex;
    } else if (region == "PHANTOM") {
      vertex = cylindric_gen_->GenerateVertex("BODY_VOL");
    } else {
      G4Exception("[FullRingInfinity]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");     
    }
    
    return vertex;
  }

}
