// ----------------------------------------------------------------------------
// nexus | Next100InnerElements.cc
//
// Inner elements of the NEXT-100 detector. They include the field cage,
// the energy and the tracking plane.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100InnerElements.h"
#include "Next100FieldCage.h"
#include "Next100EnergyPlane.h"
#include "Honeycomb.h"
#include "Next100TrackingPlane.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Material.hh>

#include <CLHEP/Units/SystemOfUnits.h>

using namespace CLHEP;


namespace nexus {


  Next100InnerElements::Next100InnerElements(G4double grid_thickn):
    GeometryBase(),
    mother_logic_(nullptr),
    mother_phys_ (nullptr),
    gas_(nullptr),
    field_cage_    (new Next100FieldCage(grid_thickn)),
    energy_plane_  (new Next100EnergyPlane()),
    honeycomb_     (new Honeycomb()),
    tracking_plane_(new Next100TrackingPlane()),
    msg_(nullptr)
  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
                                  "Control commands of geometry Next100.");
  }


  void Next100InnerElements::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }


  void Next100InnerElements::SetPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    mother_phys_ = mother_phys;
  }


  void Next100InnerElements::Construct()
  {
    G4ThreeVector coord_origin = GetCoordOrigin();
    // Reading mother material
    gas_ = mother_logic_->GetMaterial();
    pressure_ =    gas_->GetPressure();
    temperature_ = gas_->GetTemperature();

    // Field Cage
    field_cage_->SetMotherLogicalVolume(mother_logic_);
    field_cage_->SetMotherPhysicalVolume(mother_phys_);
    field_cage_->SetCoordOrigin(coord_origin);
    field_cage_->SetELtoSapphireWDWdistance(gate_sapphire_wdw_distance_);
    field_cage_->Construct();

    // Energy Plane
    energy_plane_->SetMotherLogicalVolume(mother_logic_);
    energy_plane_->SetCoordOrigin(coord_origin);
    energy_plane_->SetELtoSapphireWDWdistance(gate_sapphire_wdw_distance_);
    energy_plane_->Construct();

    pmt_pos_ = energy_plane_->GetPMTPosInGas();

    // Honeycomb support structure for EP
    honeycomb_->SetMotherLogicalVolume(mother_logic_);
    //honeycomb_->SetELzCoord(gate_zpos);
    honeycomb_->SetEndOfCopperPlateZ(energy_plane_->GetCopperPlateEndZ());
    honeycomb_->Construct();

    // Tracking plane
    tracking_plane_->SetMotherPhysicalVolume(mother_phys_);
    tracking_plane_->SetCoordOrigin(coord_origin);
    tracking_plane_->SetELtoTPdistance(gate_tracking_plane_distance_);
    tracking_plane_->Construct();

    tracking_plane_->GetSiPMPosInGas(sipm_pos_);
  }


  Next100InnerElements::~Next100InnerElements()
  {
    delete field_cage_;
    delete energy_plane_;
    delete tracking_plane_;
  }


  G4ThreeVector Next100InnerElements::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // Field Cage regions
    if ((region == "CENTER") ||
        (region == "ACTIVE") ||
        (region == "CATHODE_RING") ||
        (region == "BUFFER") ||
        (region == "XENON")  ||
        (region == "EL_GAP") ||
        (region == "LIGHT_TUBE") ||
        (region == "HDPE_TUBE") ||
        (region == "FIELD_RING") ||
        (region == "GATE_RING") ||
        (region == "ANODE_RING") ||
        (region == "RING_HOLDER")) {
      vertex = field_cage_->GenerateVertex(region);
    }
    // Energy Plane regions
    else if ((region == "EP_COPPER_PLATE") ||
             (region == "SAPPHIRE_WINDOW") ||
             (region == "OPTICAL_PAD") ||
             (region == "PMT") ||
             (region == "PMT_BODY") ||
             (region == "PMT_BASE")) {
      vertex = energy_plane_->GenerateVertex(region);
    }
    // Tracking Plane regions
    else if ((region == "TP_COPPER_PLATE") ||
             (region == "SIPM_BOARD") ||
             (region == "DB_PLUG")) {
      vertex = tracking_plane_->GenerateVertex(region);
    }
    else {
      G4Exception("[Next100InnerElements]", "GenerateVertex()", FatalException,
        "Unknown vertex generation region!");
    }

    return vertex;
  }

} // end namespace nexus
