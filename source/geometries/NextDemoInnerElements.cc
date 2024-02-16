// ----------------------------------------------------------------------------
// nexus | NextDemoInnerElements.cc
//
// Wrapper for the placement of the inner elements (field cage and photosensor
// arrays) of the NEXT-DEMO detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextDemoInnerElements.h"

#include "NextDemoFieldCage.h"
#include "NextDemoTrackingPlane.h"
#include "NextDemoEnergyPlane.h"
#include "MaterialsList.h"

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>
#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4Region.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>

using namespace nexus;


NextDemoInnerElements::NextDemoInnerElements():
  GeometryBase(),
  config_                     (""),
  gate_sapphire_wdw_distance_ (427.5 * mm),
  mother_logic_vol_           (nullptr),
  mother_phys_vol_            (nullptr),
  verbosity_                  (0)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/", "Control commands of geometry NextDemo.");

  msg_->DeclareProperty("inner_elements_verbosity", verbosity_, "Inner Elements verbosity");

  msg_->DeclareProperty("config", config_, "NextDemo configuration");

  // Build the internal objects that live there
  field_cage_     = new NextDemoFieldCage();
  tracking_plane_ = new NextDemoTrackingPlane();
  energy_plane_   = new NextDemoEnergyPlane();
}


void NextDemoInnerElements::Construct()
{
  // Make sure that the configuration has been set, and properly set
  if (config_ == "")
    G4Exception("[NextDemoInnerElements]", "Construct()",
                FatalException, "NextDemo configuration has not been set.");
  else if ((config_ != "run5") &&
           (config_ != "run7") &&
           (config_ != "run8") &&
           (config_ != "run9") &&
           (config_ != "run10"))
    G4Exception("[NextDemoInnerElements]", "Construct()",
                FatalException, "Wrong NextDemo configuration.");


  // Position in Z of the beginning of the drift region
  G4double gate_zpos = GetELzCoord();

  // Reading material
  gas_         = mother_logic_vol_->GetMaterial();
  pressure_    = gas_->GetPressure();
  temperature_ = gas_->GetTemperature();

  //INNER ELEMENTS
  field_cage_->SetMotherLogicalVolume(mother_logic_vol_);
  field_cage_->SetMotherPhysicalVolume(mother_phys_vol_);
  field_cage_->SetELzCoord(gate_zpos);
  field_cage_->SetConfig(config_);
  field_cage_->Construct();

  tracking_plane_->SetMotherPhysicalVolume(mother_phys_vol_);
  tracking_plane_->SetELzCoord(gate_zpos);
  tracking_plane_->SetConfig(config_);
  tracking_plane_->Construct();

  energy_plane_->SetMotherLogicalVolume(mother_logic_vol_);
  energy_plane_->SetELzCoord(gate_zpos);
  energy_plane_->SetGateSapphireSurfaceDistance(gate_sapphire_wdw_distance_);
  energy_plane_->Construct();
}


NextDemoInnerElements::~NextDemoInnerElements()
{
  delete field_cage_;
  delete tracking_plane_;
  delete energy_plane_;
}


G4ThreeVector NextDemoInnerElements::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex(0.,0.,0.);

  // Field Cage
  if ((region == "ACTIVE") ||
      (region == "EL_GAP")) {
    vertex = field_cage_->GenerateVertex(region);
  }

  // Tracking PLane
  else if ((region == "TP_PLATE") || (region == "SIPM_BOARD")) {
    vertex = tracking_plane_->GenerateVertex(region);
  }
  else {
    G4Exception("[NextDemoInnerElements]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }

  return vertex;
}
