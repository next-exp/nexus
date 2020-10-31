// -----------------------------------------------------------------------------
// nexus | NextDemo.cc
//
// Geometry of the NEXT-DEMO++ detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextDemo.h"

#include "NextDemoVessel.h"
#include "NextDemoInnerElements.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>

using namespace nexus;


NextDemo::NextDemo():
  BaseGeometry(),
  lab_size_(10.*m),
  vessel_geom_(new NextDemoVessel),
  inner_geom_(new NextDemoInnerElements)
{
}


NextDemo::~NextDemo()
{
  delete vessel_geom_;
  delete inner_geom_;
}


void NextDemo::Construct()
{
  ConstructLab();

  vessel_geom_->Construct();
  G4VPhysicalVolume* vessel_phys_vol =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.),
                      vessel_geom_->GetLogicalVolume(),
                      vessel_geom_->GetLogicalVolume()->GetName(),
                      BaseGeometry::GetLogicalVolume(),
                      false, 0, false);

  inner_geom_->SetMotherLogicalVolume(vessel_geom_->GetLogicalVolume());
  inner_geom_->SetMotherPhysicalVolume(vessel_phys_vol);
  inner_geom_->Construct();
}


void NextDemo::ConstructLab()
{
  // LAB /////////////////////////////////////////////////////////////

  G4String lab_name = "LAB";

  G4Box* lab_solid_vol =
    new G4Box(lab_name, lab_size_/2., lab_size_/2., lab_size_/2.);

  G4LogicalVolume* lab_logic_vol =
    new G4LogicalVolume(lab_solid_vol,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),
                        lab_name);

  // Make the volume invisible in the visualization
  lab_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(lab_logic_vol);
}


G4ThreeVector NextDemo::GenerateVertex(const G4String& /*region*/) const
{
  return G4ThreeVector();
}
