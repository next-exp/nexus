// ----------------------------------------------------------------------------
//  $Id: FullRingInfinity.cc  $
//
//  Author:  <paolafer@ific.uv.es>   
//  Created: March 2018
//  
//  Copyright (c) 2015 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "FullRingInfinity.h"
#include "QuadFBK.h"
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

#include <stdexcept>

namespace nexus {

  FullRingInfinity::FullRingInfinity():
    BaseGeometry(),
    // Detector dimensions
    lat_dimension_cell_(52.*mm),
    n_cells_(12),
    lin_n_quad_per_cell_(8),
    quad_pitch_(6.5*mm),
    kapton_thickn_(0.3*mm),
    // r_dim_(5.*cm),
    //    internal_radius_(99.3127*mm),
    cryo_width_(8.*cm),
    cryo_thickn_(1.*mm),
    max_step_size_(1.*mm)
  {
     // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/FullRingInfinity/",
                                  "Control commands of geometry FullRingInfinity.");

    quad_ = new QuadFBK();
   
    phantom_diam_ = 12.*cm;
    phantom_length_ = 10.*cm;

    internal_radius_ = n_cells_ * lin_n_quad_per_cell_ * quad_pitch_ / (2*pi);
    G4int n_ext_cells = 1.5 * n_cells_;
    external_radius_ = 1.5 * internal_radius_;

    //G4cout << internal_radius_/cm << ", " << external_radius_/cm << ", " << G4endl;
    
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

    BuildCryostat();
    BuildSensors();
  }

  void FullRingInfinity::BuildCryostat()
  {
    const G4double space_for_elec = 2. * cm;
    const G4double int_radius_cryo = internal_radius_ - cryo_thickn_ - space_for_elec;
    const G4double ext_radius_cryo = external_radius_ + cryo_thickn_ + space_for_elec;
   

    G4Tubs* cryostat_solid =
      new G4Tubs("CRYOSTAT", int_radius_cryo, ext_radius_cryo, cryo_width_/2., 0, twopi);
    G4Material* steel = MaterialsList::Steel();
    G4LogicalVolume* cryostat_logic =
      new G4LogicalVolume(cryostat_solid, steel, "CRYOSTAT");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), cryostat_logic,
		      "CRYOSTAT", lab_logic_, false, 0, true);

    G4double ext_offset = 1. * mm;
    G4Tubs* LXe_solid =
      new G4Tubs("LXE", internal_radius_ - kapton_thickn_, external_radius_ + ext_offset + kapton_thickn_,
                 (cryo_width_ - 2.*cryo_thickn_)/2., 0, twopi);
    G4Material* LXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    LXe->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    LXe_logic_ =
      new G4LogicalVolume(LXe_solid, LXe, "LXE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), LXe_logic_,
		      "LXE", cryostat_logic, false, 0, true);

    G4Tubs* active_solid =
      new G4Tubs("ACTIVE", internal_radius_, external_radius_ + ext_offset,
                 (cryo_width_ - 2.*cryo_thickn_)/2., 0, twopi);
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
      new G4Tubs("KAPTON", internal_radius_ - kapton_thickn_, internal_radius_,
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

    // G4cout << (external_radius_  - kapton_thickn_) / cm << G4endl;

    G4VisAttributes kapton_col = nexus::CopperBrown();
    kapton_col.SetForceSolid(true);
    kapton_int_logic->SetVisAttributes(kapton_col);
    kapton_ext_logic->SetVisAttributes(kapton_col);
    // G4VisAttributes active_col = nexus::Blue();
    // active_col.SetForceSolid(true);
    // active_logic->SetVisAttributes(active_col);

  }

  void FullRingInfinity::BuildSensors()
  {

    /// Build internal array first
    quad_->Construct();
    G4LogicalVolume* quad_logic = quad_->GetLogicalVolume();
    G4ThreeVector quad_dim = quad_->GetDimensions();

    G4int n_quad_int = 2*pi*internal_radius_/quad_pitch_;
    G4cout << "Number of quads in internal: " <<  n_quad_int << G4endl;
    G4double step = 2.*pi/n_quad_int;
    G4double radius = internal_radius_ + kapton_thickn_ + quad_dim.z()/2.;
  
    G4RotationMatrix rot;
    rot.rotateX(-pi/2.);
    
    G4int copy_no = 0;
    for (G4int j=0; j<lin_n_quad_per_cell_; j++) {
      // The first must be positioned outside the loop
      if (j!=0) rot.rotateZ(step);
      G4double z_dimension = -lat_dimension_cell_/2. + j * quad_pitch_;
      G4ThreeVector position(0., radius, z_dimension);
      G4String vol_name = "QUAD_" + std::to_string(copy_no);
      copy_no += 1;
      new G4PVPlacement(G4Transform3D(rot, position), quad_logic,
                        vol_name, active_logic_, false, copy_no, true);

      for (G4int i=2; i<=n_quad_int; ++i) {
        G4double angle = (i-1)*step;
        rot.rotateZ(step);
        position.setX(-radius*sin(angle));
        position.setY(radius*cos(angle));
        copy_no += 1;
        vol_name = "QUAD_" + std::to_string(copy_no);
        new G4PVPlacement(G4Transform3D(rot, position), quad_logic,
                          vol_name, active_logic_, false, copy_no, true);
      }
    }

    G4int n_quad_ext = 2*pi*external_radius_/quad_pitch_;
    step = 2.*pi/n_quad_ext;
    radius = external_radius_ - quad_dim.z()/2.;
    
    rot.rotateZ(step);
    rot.rotateX(pi);
    
    //copy_no = 2000;
    for (G4int j=0; j<lin_n_quad_per_cell_; j++) {
      // The first must be positioned outside the loop
      if (j!=0) rot.rotateZ(step);
      G4double z_pos = -lat_dimension_cell_/2. + (j + 1./2.) * quad_pitch_;
      G4ThreeVector position(0., radius, z_pos);
      copy_no = copy_no + 1;
      G4String vol_name = "QUAD_" + std::to_string(copy_no);
      new G4PVPlacement(G4Transform3D(rot, position), quad_logic,
                        vol_name, active_logic_, false, copy_no, false);

      for (G4int i=2; i<=n_quad_ext; ++i) {
        G4double angle = (i-1)*step;
        rot.rotateZ(step);
        position.setX(-radius*sin(angle));
        position.setY(radius*cos(angle));
        copy_no = copy_no + 1;
        vol_name = "QUAD_" + std::to_string(copy_no);
        new G4PVPlacement(G4Transform3D(rot, position), quad_logic,
                          vol_name, active_logic_, false, copy_no, false);
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
