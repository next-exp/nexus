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
#include "LSCHallA.h"
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
    bottom_nozzle_ypos_(-53. * cm),
    lab_walls_(false)
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

    msg_->DeclareProperty("lab_walls", lab_walls_, "Placement of Hall A walls");


  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones

  // Shielding
  shielding_ = new Next100Shielding();

  //Lab walls
  hallA_walls_ = new LSCHallA();

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
    delete hallA_walls_;
  }



  void Next100::Construct()
  {
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air surrounding the detector so that
    // events (from calibration sources or cosmic rays) can be generated
    // on the outside.
    if (lab_walls_){
      // We want to simulate the walls (for muons in most cases).
      hallA_walls_->Construct();
      hallA_logic_ = hallA_walls_->GetLogicalVolume();
      G4double hallA_length = hallA_walls_->GetLSCHallALength();
      // Since the walls will be displaced need to make the
      // "lab" double sized to be sure.
      G4Box* lab_solid = new G4Box("LAB", hallA_length,
				   hallA_length, hallA_length);
      G4Material *vacuum =
	G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
      lab_logic_ = new G4LogicalVolume(lab_solid, vacuum, "LAB");
      this->SetSpan(2 * hallA_length);
    } else {
      G4Box* lab_solid =
	new G4Box("LAB", lab_size_/2., lab_size_/2., lab_size_/2.);

      lab_logic_ =
	new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),
			    "LAB");
    }
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

    G4ThreeVector gate_pos(0., 0., -gate_zpos_in_vessel_);
    if (lab_walls_){
      G4ThreeVector castle_pos(0., hallA_walls_->GetLSCHallACastleY(),
			       hallA_walls_->GetLSCHallACastleZ());
      new G4PVPlacement(0, castle_pos, shielding_logic, "LEAD_BOX",
       			hallA_logic_, false, 0);
      new G4PVPlacement(0, gate_pos - castle_pos, hallA_logic_, "Hall_A",
      			lab_logic_, false, 0, false);
    } else {
      new G4PVPlacement(0, gate_pos, shielding_logic,
			"LEAD_BOX", lab_logic_, false, 0);
    }


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
    // Lab walls
    else if ((region == "HALLA_INNER") || (region == "HALLA_OUTER")){
      if (!lab_walls_)
	G4Exception("[Next100]", "GenerateVertex()", FatalException,
                    "This vertex generation region must be used with lab_walls == true!");
      vertex = hallA_walls_->GenerateVertex(region);
    }
    else {
      G4Exception("[Next100]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    G4ThreeVector displacement = G4ThreeVector(0., 0., -gate_zpos_in_vessel_);
    vertex = vertex + displacement;

    return vertex;
  }


  G4ThreeVector Next100::ProjectToRegion(const G4String& region,
					 const G4ThreeVector& point,
					 const G4ThreeVector& dir) const
  {
    // Project along dir from point to find the first intersection
    // with region.
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "EXTERNAL"){
      return shielding_->ProjectToRegion(region, point, dir);
    }
    else if ((region == "HALLA_OUTER") || (region == "HALLA_INNER")){
      if (!lab_walls_)
	G4Exception("[Next100]", "ProjectToRegion()", FatalException,
                    "To project to this region you need lab_walls == true!");
      return hallA_walls_->ProjectToRegion(region, point, dir);
    }
    else {
      G4Exception("[Next100]", "ProjectToRegion()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex + G4ThreeVector(0., 0., -gate_zpos_in_vessel_);
  }


} //end namespace nexus
