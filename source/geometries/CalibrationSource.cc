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
    _source_diam (6.*mm),
    _source_thick (2.*mm),
    _capsule_diam (7.*mm),
    _capsule_thick (16.*mm),
    _source_z_pos(0.*mm),
    _source("Na")
  {
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/CalibrationSource/", "Control commands of geometry CalibrationSource.");
     _msg->DeclareProperty("source", _source, "Radioactive source being used");
  }
  
  CalibrationSource::~CalibrationSource()
  {
    
  }
  
  void CalibrationSource::Construct()
  {
    // G4double piece_diam = 7. *mm;
    // G4double piece_length = 16. *mm;
    G4Tubs* screw_tube_solid =
      new G4Tubs("SCREW_SUPPORT", 0., _capsule_diam/2., _capsule_thick/2., 0, twopi);
    G4LogicalVolume* screw_tube_logic =
      new G4LogicalVolume(screw_tube_solid,  G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "SCREW_SUPPORT");

    this->SetLogicalVolume(screw_tube_logic);
  
    // G4double source_diam = 6. * mm;
    // G4double source_thickness = 2. * mm;
   
    G4Tubs* source_solid = 
      new G4Tubs("SCREW_SOURCE", 0., _source_diam/2., _source_thick/2., 0., twopi);

    G4String material = "G4_" + _source;
  
    G4Material* source_mat = 
      G4NistManager::Instance()->FindOrBuildMaterial(material);
    G4LogicalVolume* source_logic = 
      new G4LogicalVolume(source_solid, source_mat, "SCREW_SOURCE");

    _source_z_pos = _capsule_thick/2. - 0.5 * mm - _source_thick/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., _source_z_pos),
		      source_logic, "SCREW_SOURCE", 
		      screw_tube_logic, false, 0, true);

    
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
    return _source_diam;
  }

  G4double CalibrationSource::GetSourceThickness()
  {
    return _source_thick;
  }

   G4double CalibrationSource::GetCapsuleDiameter()
  {
    return _capsule_diam;
  }

  G4double CalibrationSource::GetCapsuleThickness()
  {
    return _capsule_thick;
  }

  G4double CalibrationSource::GetSourceZpos()
  {
    return _source_z_pos;
  }
  
}
