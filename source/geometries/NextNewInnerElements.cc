// -----------------------------------------------------------------------------
// nexus | NextNewInnerElements.cc
//
// Inner elements of the NEXT-WHITE detector. They include the field cage,
// the energy plane and the tracking plane.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextNewInnerElements.h"
#include "NextNewEnergyPlane.h"
#include "NextNewFieldCage.h"
#include "NextNewTrackingPlane.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextNewInnerElements::NextNewInnerElements():
    BaseGeometry(),
    mother_logic_(nullptr), mother_phys_(nullptr)
  {
    // Build the internal objects that live there
    energy_plane_ = new NextNewEnergyPlane();
    field_cage_ = new NextNewFieldCage();
    tracking_plane_ = new NextNewTrackingPlane();
  }

  void NextNewInnerElements::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }

  void NextNewInnerElements::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    mother_phys_ = mother_phys;
  }

  void NextNewInnerElements::Construct()
  {
    // Reading material
    gas_ = mother_logic_->GetMaterial();
    pressure_ =    gas_->GetPressure();
    temperature_ = gas_->GetTemperature();
    //INNER ELEMENTS
    field_cage_->SetMotherLogicalVolume(mother_logic_);
    field_cage_->SetMotherPhysicalVolume(mother_phys_);
    field_cage_->Construct();
    SetELzCoord(field_cage_->GetELzCoord());
    tracking_plane_->SetLogicalVolume(mother_logic_);
    tracking_plane_->SetELzCoord(field_cage_->GetELzCoord());
    tracking_plane_->Construct();
    energy_plane_->SetLogicalVolume(mother_logic_);
    energy_plane_->SetELzCoord(field_cage_->GetELzCoord());
    energy_plane_->Construct();
  }

  NextNewInnerElements::~NextNewInnerElements()
  {
    delete energy_plane_;
    delete field_cage_;
    delete tracking_plane_;
  }


  G4ThreeVector NextNewInnerElements::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // INNER ELEMENTS regions
    if ( (region == "CARRIER_PLATE")      || (region == "ENCLOSURE_BODY") ||
         (region == "ENCLOSURE_WINDOW")   || (region == "OPTICAL_PAD") ||
         (region == "PMT_BODY")           || (region == "PMT_BASE") ||
         (region == "INT_ENCLOSURE_SURF") || (region == "PMT_SURF")) {
      vertex = energy_plane_->GenerateVertex(region);
    }
    else if ( (region == "DRIFT_TUBE") ||
              (region == "HDPE_TUBE") ||
              (region == "ANODE_QUARTZ") ||
              (region == "CENTER") ||
              (region == "CATHODE") ||
              (region == "TRACKING_FRAMES") ||
              (region == "XENON") ||
              (region == "ACTIVE") ||
              (region == "BUFFER") ||
              (region == "EL_GAP") ||
              (region == "EL_TABLE") ||
              (region == "AD_HOC")) {
      vertex = field_cage_->GenerateVertex(region);
    }
    else if ( (region == "SUPPORT_PLATE") || (region == "DICE_BOARD") || (region == "DB_PLUG") ) {
      vertex = tracking_plane_->GenerateVertex(region);
    }
    else {
      G4Exception("[NextNewInnerElements]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }
    return vertex;
  }
}//end namespace nexus
