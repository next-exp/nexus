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
  specific_vertex_X_(0.), specific_vertex_Y_(0.), specific_vertex_Z_(0.),
  vessel_geom_(new NextDemoVessel),
  inner_geom_(new NextDemoInnerElements),
  msg_(nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/",
                                "Control commands of geometry NextDemo.");

  G4GenericMessenger::Command&  specific_vertex_X_cmd =
    msg_->DeclareProperty("specific_vertex_X", specific_vertex_X_,
                          "If region is AD_HOC, x coord where particles are generated");
  specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
  specific_vertex_X_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command&  specific_vertex_Y_cmd =
    msg_->DeclareProperty("specific_vertex_Y", specific_vertex_Y_,
                          "If region is AD_HOC, y coord where particles are generated");
  specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
  specific_vertex_Y_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command&  specific_vertex_Z_cmd =
    msg_->DeclareProperty("specific_vertex_Z", specific_vertex_Z_,
                            "If region is AD_HOC, z coord where particles are generated");
  specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
  specific_vertex_Z_cmd.SetUnitCategory("Length");

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


G4ThreeVector NextDemo::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vtx;

  if (region == "AD_HOC") {
    vtx = G4ThreeVector(specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_);
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
