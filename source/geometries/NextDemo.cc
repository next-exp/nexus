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
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>

using namespace nexus;

REGISTER_CLASS(NextDemo, GeometryBase)


NextDemo::NextDemo():
  GeometryBase(),
  lab_size_(10.*m),
  specific_vertex_{},
  vessel_geom_(new NextDemoVessel),
  inner_geom_(new NextDemoInnerElements),
  msg_(nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/",
                                "Control commands of geometry NextDemo.");

  msg_->DeclarePropertyWithUnit("specific_vertex", "mm",  specific_vertex_,
      "Set generation vertex.");

}


NextDemo::~NextDemo()
{
  delete vessel_geom_;
  delete inner_geom_;
  delete msg_;
}


void NextDemo::Construct()
{
  ConstructLab();

  vessel_geom_->Construct();
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -vessel_geom_->GetGateEndcapDistance()),
                    vessel_geom_->GetLogicalVolume(),
                    vessel_geom_->GetLogicalVolume()->GetName(),
                    GeometryBase::GetLogicalVolume(),
                    false, 0, false);

  inner_geom_->SetELzCoord(vessel_geom_->GetGateEndcapDistance());
  inner_geom_->SetMotherLogicalVolume(vessel_geom_->GetGasPhysicalVolume()->GetLogicalVolume());
  inner_geom_->SetMotherPhysicalVolume(vessel_geom_->GetGasPhysicalVolume());
  inner_geom_->Construct();
}


void NextDemo::ConstructLab()
{
  // LAB /////////////////////////////////////////////////////////////

  G4String lab_name = "LAB";

  G4Box* lab_solid_vol =
    new G4Box(lab_name, lab_size_/2., lab_size_/2., lab_size_/2.);

  G4Material* air_mat = 
    G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  air_mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4LogicalVolume* lab_logic_vol =
    new G4LogicalVolume(lab_solid_vol,
                        air_mat,
                        lab_name);

  // Make the volume invisible in the visualization
  lab_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(lab_logic_vol);
}


G4ThreeVector NextDemo::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vtx;

  if (region == "AD_HOC") {
    vtx = specific_vertex_;
  }
  else if (region == "CALIBRATION_SOURCE") {
    vtx = vessel_geom_->GenerateVertex("CALIBRATION_SOURCE");
    G4ThreeVector displacement = G4ThreeVector(0., 0., -vessel_geom_->GetGateEndcapDistance());
    vtx = vtx + displacement;
  }
  else if ((region == "ACTIVE")  ||
           (region == "TP_PLATE") ||
           (region == "SIPM_BOARD") ||
           (region == "EL_GAP")) {
    vtx = inner_geom_->GenerateVertex(region);
    G4ThreeVector displacement = G4ThreeVector(0., 0., -vessel_geom_->GetGateEndcapDistance());
    vtx = vtx + displacement;
  }
  else {
    G4Exception("[NextDemo]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region.");
  }

  return vtx;
}
