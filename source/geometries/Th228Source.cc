#include "Th228Source.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4Tubs.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  Th228Source::Th228Source():
    DiskSource()
  {
    _source_diam = 5.*mm;
    _source_thick = .1*mm;
    _support_diam = 12.7*mm;
    _support_thick = 6.35*mm;
  }
  
  Th228Source::~Th228Source()
  {
    
  }
  
  void Th228Source::Construct()
  {

    ///Plastic support
    G4Tubs* support_solid = 
      new G4Tubs("SUPPORT", 0., _support_diam/2., _support_thick/2., 0., twopi);

    G4Material* plastic = MaterialsList::PS();
    G4LogicalVolume* support_logic = 
      new G4LogicalVolume(support_solid, plastic, "TH228_SOURCE_SUPPORT");
    
    this->SetLogicalVolume(support_logic);
    
    // G4double source_thick = .1*mm;
    // G4double source_diam = 3.*mm;
    G4Tubs* source_solid = 
      new G4Tubs("SOURCE", 0., _source_diam/2., _source_thick/2., 0., twopi);
    G4Material* sodium22_mat = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Th");
    G4LogicalVolume* source_logic = 
      new G4LogicalVolume(source_solid, sodium22_mat, "TH228");
    
    new G4PVPlacement(0, G4ThreeVector(0., 0.,  _support_thick/2. - _source_thick/2.),
		      source_logic, "TH228",
		      support_logic, false, 0, false);
    
    G4VisAttributes source_col = nexus::DarkGreen();
    source_col.SetForceSolid(true);
    source_logic->SetVisAttributes(source_col);
    
    return;
  
  }

  // G4double Th228Source::GetSourceDiameter()
  // {
  //   return _source_diam;
  // }

  // G4double Th228Source::GetSourceThickness()
  // {
  //   return _source_thick;
  // }

  //  G4double Th228Source::GetSupportDiameter()
  // {
  //   return _support_diam;
  // }

  // G4double Th228Source::GetSupportThickness()
  // {
  //   return _support_thick;
  // }
  
}
