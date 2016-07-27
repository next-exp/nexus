#include "Na22Source.h"
#include "MaterialsList.h"

#include <G4Tubs.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  Na22Source::Na22Source():
    BaseGeometry(),
    _source_diam (3.*mm),
    _source_thick (.1*mm),
    _support_diam (25.33*mm),
    _support_thick (6.2*mm)
  {
    
  }
  
  Na22Source::~Na22Source()
  {
    
  }
  
  void Na22Source::Construct()
  {

    ///Plastic support
    // G4double support_thick = 6.2*mm;
    // G4double support_diam = 25.33*mm;
    G4Tubs* support_solid = 
      new G4Tubs("SUPPORT", 0., _support_diam/2., _support_thick/2., 0., twopi);

    G4Material* plastic = MaterialsList::PS();
    G4LogicalVolume* support_logic = 
      new G4LogicalVolume(support_solid, plastic, "NA22_SOURCE_SUPPORT");
    
    this->SetLogicalVolume(support_logic);
    
    // G4double source_thick = .1*mm;
    // G4double source_diam = 3.*mm;
    G4Tubs* source_solid = 
      new G4Tubs("SOURCE", 0., _source_diam/2., _source_thick/2., 0., twopi);
    G4Material* sodium22_mat = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Na");
    G4LogicalVolume* source_logic = 
      new G4LogicalVolume(source_solid, sodium22_mat, "NA22");
    
    new G4PVPlacement(0, G4ThreeVector(0., 0.,  _support_thick/2. - _source_thick/2.),
		      source_logic, "NA22",
		      support_logic, false, 0, false);
    G4VisAttributes * vis_green = new G4VisAttributes;
    vis_green->SetColor(0., 1., 0.);
    vis_green->SetForceSolid(true);
    source_logic->SetVisAttributes(vis_green);

    
    
    return;
  
  }

  G4double Na22Source::GetSourceDiameter()
  {
    return _source_diam;
  }

  G4double Na22Source::GetSourceThickness()
  {
    return _source_thick;
  }

   G4double Na22Source::GetSupportDiameter()
  {
    return _support_diam;
  }

  G4double Na22Source::GetSupportThickness()
  {
    return _support_thick;
  }
  
}
