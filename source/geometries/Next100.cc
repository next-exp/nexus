// ----------------------------------------------------------------------------
// nexus | Next100.cc
//
// Main class that constructs the geometry of the NEXT-100 detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100.h"
#include "BoxPointSampler.h"
#include "MuonsPointSampler.h"
#include "Next100Shielding.h"
#include "Next100Vessel.h"
#include "Next100Ics.h"
#include "Next100InnerElements.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4UserLimits.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  Next100::Next100():
    BaseGeometry(),
    // Lab dimensions
    lab_size_ (5. * m),
    // Nozzles external diam and y positions
    nozzle_ext_diam_ (9. * cm),
    up_nozzle_ypos_ (20. * cm),
    central_nozzle_ypos_ (0. * cm),
    down_nozzle_ypos_ (-20. * cm),
    bottom_nozzle_ypos_(-53. * cm)
  {

    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");

    G4GenericMessenger::Command&  specific_vertex_X_cmd =
      msg_->DeclareProperty("specific_vertex_X", specific_vertex_X_,
                            "If region is AD_HOC, x coord of primary particles");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
      msg_->DeclareProperty("specific_vertex_Y", specific_vertex_Y_,
                            "If region is AD_HOC, y coord of primary particles");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
      msg_->DeclareProperty("specific_vertex_Z", specific_vertex_Z_,
                            "If region is AD_HOC, z coord of primary particles");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");


  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones

  // Shielding
  shielding_ = new Next100Shielding();

  // Vessel
  vessel_ = new Next100Vessel(nozzle_ext_diam_, up_nozzle_ypos_, central_nozzle_ypos_,
			      down_nozzle_ypos_, bottom_nozzle_ypos_);

  // Internal copper shielding
  ics_ = new Next100Ics(nozzle_ext_diam_, up_nozzle_ypos_, central_nozzle_ypos_,
			down_nozzle_ypos_, bottom_nozzle_ypos_);

  // Inner Elements
  inner_elements_ = new Next100InnerElements();

  }



  Next100::~Next100()
  {
    delete inner_elements_;
    delete ics_;
    delete vessel_;
    delete shielding_;
    delete lab_gen_;
  }



  void Next100::Construct()
  {
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air surrounding the detector so that
    // events (from calibration sources or cosmic rays) can be generated
    // on the outside.

    G4Box* lab_solid =
      new G4Box("LAB", lab_size_/2., lab_size_/2., lab_size_/2.);

    lab_logic_ =
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),
			  "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);

    // Set this volume as the wrapper for the whole geometry
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(lab_logic_);


    // SHIELDING
    shielding_->Construct();
    G4LogicalVolume* shielding_logic = shielding_->GetLogicalVolume();

    // VESSEL
    vessel_->Construct();
    G4LogicalVolume* shielding_air_logic = shielding_->GetAirLogicalVolume();
    G4LogicalVolume* vessel_logic = vessel_->GetLogicalVolume();
    gate_zpos_in_vessel_ = vessel_->GetELzCoord();
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), vessel_logic,
    		      "VESSEL", shielding_air_logic, false, 0);

    G4LogicalVolume* vessel_internal_logic = vessel_->GetInternalLogicalVolume();
    G4VPhysicalVolume* vessel_internal_phys = vessel_->GetInternalPhysicalVolume();

    // Inner Elements
    inner_elements_->SetLogicalVolume(vessel_internal_logic);
    inner_elements_->SetPhysicalVolume(vessel_internal_phys);
    inner_elements_->SetELzCoord(gate_zpos_in_vessel_);
    inner_elements_->Construct();

    // Internal Copper Shielding
    ics_->SetLogicalVolume(vessel_internal_logic);
    ics_->Construct();

    new G4PVPlacement(0, G4ThreeVector(0., 0., -gate_zpos_in_vessel_), shielding_logic,
     		      "LEAD_BOX", lab_logic_, false, 0);


    //// VERTEX GENERATORS   //
    lab_gen_ =
      new BoxPointSampler(lab_size_ - 1.*m, lab_size_ - 1.*m, lab_size_  - 1.*m,
			  1.*m,G4ThreeVector(0., 0., 0.), 0);
  }



  G4ThreeVector Next100::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // Air around shielding
    if (region == "LAB") {
      vertex = lab_gen_->GenerateVertex("INSIDE");
    }
    // Shielding regions
    else if ((region == "SHIELDING_LEAD")  ||
             (region == "SHIELDING_STEEL") ||
             (region == "EXTERNAL") ||
             (region == "INNER_AIR") ||
             (region == "SHIELDING_STRUCT") ) {
      vertex = shielding_->GenerateVertex(region);
    }
    // Vessel regions
    else if ((region == "VESSEL") ||
	     (region == "VESSEL_FLANGES") ||
	     (region == "VESSEL_TRACKING_ENDCAP") ||
	     (region == "VESSEL_ENERGY_ENDCAP")) {
      vertex = vessel_->GenerateVertex(region);
    }
    // Inner copper shielding
    else if ((region == "ICS") ||
	     (region == "DB_PLUG")) {
      vertex = ics_->GenerateVertex(region);
    }
    // Inner elements (photosensors' planes and field cage)
    else if ((region == "CENTER") ||
	     (region == "ACTIVE") ||
	     (region == "BUFFER") ||
	     (region == "XENON") ||
	     (region == "LIGHT_TUBE") ||
	     (region == "EP_COPPER_PLATE") ||
	     (region == "SAPPHIRE_WINDOW") ||
	     (region == "OPTICAL_PAD") ||
	     (region == "PMT_BODY") ||
	     (region == "PMT") ||
	     (region == "INTERNAL_PMT_BASE") ||
	     (region == "EXTERNAL_PMT_BASE") ||
	     (region == "TP_COPPER_PLATE") ||
	     (region == "DICE_BOARD") ||
	     (region == "AXIAL_PORT") ||
	     (region == "EL_TABLE") ) {
      vertex = inner_elements_->GenerateVertex(region);
    }
    else if (region == "AD_HOC") {
      // AD_HOC does not need to be shifted because it is passed by the user
      vertex =
	G4ThreeVector(specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_);
      return vertex;
    }
    else {
      G4Exception("[Next100]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    G4ThreeVector displacement = G4ThreeVector(0., 0., -gate_zpos_in_vessel_);
    vertex = vertex + displacement;

    return vertex;
  }


} //end namespace nexus
