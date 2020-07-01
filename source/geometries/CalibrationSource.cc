// ----------------------------------------------------------------------------
// nexus | CalibrationSource.cc
//
// This class describes a source of configurable size.
// The isotope is also configurable. It is used to simulate the small sources,
// shaped as capsules, used in Canfranc.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "CalibrationSource.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4Tubs.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  CalibrationSource::CalibrationSource():
    BaseGeometry(),
    source_diam_ (6.*mm),
    source_thick_ (2.*mm),
    capsule_diam_ (7.*mm),
    capsule_thick_ (16.*mm),
    source_z_pos_(0.*mm),
    source_("Na")
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/CalibrationSource/", "Control commands of geometry CalibrationSource.");
     msg_->DeclareProperty("source", source_, "Radioactive source being used");
  }
  
  CalibrationSource::~CalibrationSource()
  {
    
  }
  
  void CalibrationSource::Construct()
  {
    // G4double piece_diam = 7. *mm;
    // G4double piece_length = 16. *mm;
    G4Tubs* screw_tube_solid =
      new G4Tubs("SCREW_SUPPORT", 0., capsule_diam_/2., capsule_thick_/2., 0, twopi);
    G4LogicalVolume* screw_tube_logic =
      new G4LogicalVolume(screw_tube_solid,  G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "SCREW_SUPPORT");

    this->SetLogicalVolume(screw_tube_logic);
  
    // G4double source_diam = 6. * mm;
    // G4double source_thickness = 2. * mm;
   
    G4Tubs* source_solid = 
      new G4Tubs("SCREW_SOURCE", 0., source_diam_/2., source_thick_/2., 0., twopi);

    G4String material = "G4_" + source_;
  
    G4Material* source_mat = 
      G4NistManager::Instance()->FindOrBuildMaterial(material);
    G4LogicalVolume* source_logic = 
      new G4LogicalVolume(source_solid, source_mat, "SCREW_SOURCE");

    source_z_pos_ = capsule_thick_/2. - 0.5 * mm - source_thick_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., source_z_pos_),
		      source_logic, "SCREW_SOURCE", 
		      screw_tube_logic, false, 0, false);

    
    G4VisAttributes vis_green = nexus::LightGreen();
    G4VisAttributes red_col = nexus::Red();
    red_col.SetForceSolid(true);
    //vis_green.SetForceSolid(true);
    screw_tube_logic->SetVisAttributes(vis_green);   
    source_logic->SetVisAttributes(red_col);
    
    return;
  
  }

  G4double CalibrationSource::GetSourceDiameter()
  {
    return source_diam_;
  }

  G4double CalibrationSource::GetSourceThickness()
  {
    return source_thick_;
  }

   G4double CalibrationSource::GetCapsuleDiameter()
  {
    return capsule_diam_;
  }

  G4double CalibrationSource::GetCapsuleThickness()
  {
    return capsule_thick_;
  }

  G4double CalibrationSource::GetSourceZpos()
  {
    return source_z_pos_;
  }
  
}
