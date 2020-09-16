// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 18 Sept 2013
//
//  Copyright (c) 2013 NEXT Collaboration
//
//  Original NextNewInnerElements.cc
//
//  Updated to NextDemo++  by  Ruth Weiss Babai  <ruty.wb@gmail.com>
//  From: NextNewInnerElements.cc
//  Date:      June-Aug 2019
// ----------------------------------------------------------------------------

#include "NextDemoInnerElements.h"
#include "NextDemoFieldCage.h"
//#include "NextDemoTrackingPlane.h"
#include "NextDemoTrackingPlane1.h"
#include "NextDemoEnergyPlane.h"
#include "MaterialsList.h"
#include "CylinderPointSampler2020.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4Region.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

namespace nexus {

  using namespace CLHEP;

  NextDemoInnerElements::NextDemoInnerElements(const G4double vessel_length):
    BaseGeometry(),
    gate_sapphire_wdw_distance_(427.5 * mm),
    gate_tp_copper_distance_(10.8 * mm + 5.79 * mm), // to be checked
    mother_logic_(nullptr),
    mother_phys_(nullptr),
    verbosity_(0)

  {

    msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/", "Control commands of geometry NextDemo.");
    msg_->DeclareProperty("inner_elements_verbosity", verbosity_, "Inner Elements verbosity");

   /// Needed External variables
    vessel_length_ = vessel_length;

    // Build the internal objects that live there
    field_cage_ = new NextDemoFieldCage();
    tracking_plane_ = new NextDemoTrackingPlane();
    energy_plane_ = new NextDemoEnergyPlane();
  }

  void NextDemoInnerElements::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }

  void NextDemoInnerElements::SetPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    mother_phys_ = mother_phys;
  }

  void NextDemoInnerElements::Construct()
  {
    // Position in Z of the beginning of the drift region
    G4double gate_zpos = GetELzCoord();

    // Reading material
    gas_         = mother_logic_->GetMaterial();
    pressure_    = gas_->GetPressure();
    temperature_ = gas_->GetTemperature();

    //INNER ELEMENTS
    field_cage_->SetMotherLogicalVolume(mother_logic_);
    field_cage_->SetMotherPhysicalVolume(mother_phys_);
    field_cage_->SetELzCoord(gate_zpos);
    field_cage_->Construct();

    tracking_plane_->SetMotherPhysicalVolume(mother_phys_);
    tracking_plane_->SetELzCoord(gate_zpos);
    //tracking_plane_->SetTPGateDistance(gate_tp_copper_distance_);
    tracking_plane_->Construct();

    energy_plane_->SetMotherLogicalVolume(mother_logic_);
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
      // (region == "DRIFT_TUBE") ||
	    // (region == "ANODE_QUARTZ") ||
	    // (region == "CENTER") ||
	    // (region == "CATHODE") ||
	    // (region == "XENON") ||
	    // (region == "BUFFER") ||
	     (region == "EL_TABLE") ||
	     (region == "AD_HOC")) {
      vertex = field_cage_->GenerateVertex(region);
    }
    // Tracking PLane
    else if ((region == "TP_PLATE") ||
             (region == "SIPM_BOARD")) {
      vertex = tracking_plane_->GenerateVertex(region);
    }
    else {
      G4Exception("[NextDemoInnerElements]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }
    return vertex;
  }

}//end namespace nexus
