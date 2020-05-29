// ----------------------------------------------------------------------------
// nexus | DetectorConstruction.cc
//
// This class is used to initialize the detector geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DetectorConstruction.h"

#include "BaseGeometry.h"

#include <G4Box.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>


using namespace nexus;



DetectorConstruction::DetectorConstruction(): geometry_(0)
{
}



DetectorConstruction::~DetectorConstruction()
{
  delete geometry_;
}



G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Check whether a detector geometry has been set
  if (!geometry_) {
    G4Exception("[DetectorConstruction]", "Construct()",
      FatalException, "Geometry not set!");
  }

  // At this point the user should have loaded the configuration
  // parameters of the geometry or it will get built with the
  // default values.
  geometry_->Construct();

  // We define now the world volume as an empty box big enough
  // to fit the user's geometry inside.

  G4double size = geometry_->GetSpan();

  G4Box* world_solid = new G4Box("WORLD", size/2., size/2., size/2.);

  G4Material* vacuum =
  G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

  G4LogicalVolume* world_logic =
  new G4LogicalVolume(world_solid, vacuum, "WORLD", 0, 0, 0, true);

  world_logic->SetVisAttributes(G4VisAttributes::Invisible);

  G4PVPlacement* world_physi =
  new G4PVPlacement(0, G4ThreeVector(), world_logic, "WORLD", 0, false, 0);

  // We place the user's geometry in the center of the world

  G4LogicalVolume* geometry_logic = geometry_->GetLogicalVolume();

  new G4PVPlacement(0, G4ThreeVector(0,0,0),
		    geometry_logic, geometry_logic->GetName(), world_logic, false, 0);

  return world_physi;
}
