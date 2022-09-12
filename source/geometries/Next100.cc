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
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4UserLimits.hh>


namespace nexus {

  REGISTER_CLASS(Next100, GeometryBase)

  using namespace CLHEP;

  Next100::Next100():
    GeometryBase(),
    // Lab dimensions
    lab_size_ (5. * m),

    // common used variables in geomety components
    // 0.1 mm grid thickness
    // note that if grid thickness change it must be also changed in Next100FieldCage.cc
    gate_tracking_plane_distance_((26.1 + 0.1)   * mm),
    gate_sapphire_wdw_distance_  ((1458.2 - 0.1) * mm),

    specific_vertex_{},
    lab_walls_(false)
  {

    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");

    msg_->DeclarePropertyWithUnit("specific_vertex", "mm",  specific_vertex_,
      "Set generation vertex.");

    msg_->DeclareProperty("lab_walls", lab_walls_, "Placement of Hall A walls");

  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones

  // Shielding
  shielding_ = new Next100Shielding();

  //Lab walls
  hallA_walls_ = new LSCHallA();

  // Vessel
  vessel_ = new Next100Vessel();

  // Internal copper shielding
  ics_ = new Next100Ics();

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
      G4Box* lab_solid = new G4Box("LAB", hallA_length, hallA_length, hallA_length);
      G4Material *vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
      lab_logic_ = new G4LogicalVolume(lab_solid, vacuum, "LAB");
      this->SetSpan(2 * hallA_length);
    }
    else {
      G4Box* lab_solid = new G4Box("LAB", lab_size_/2., lab_size_/2., lab_size_/2.);
      lab_logic_ = new G4LogicalVolume(lab_solid,
        G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    }
    lab_logic_->SetVisAttributes(G4VisAttributes::GetInvisible());

    // Set this volume as the wrapper for the whole geometry
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(lab_logic_);

    // VESSEL (initialize first since it defines EL position)
    vessel_->SetELtoTPdistance(gate_tracking_plane_distance_);
    vessel_->Construct();
    G4LogicalVolume* vessel_logic = vessel_->GetLogicalVolume();
    G4LogicalVolume* vessel_internal_logic  = vessel_->GetInternalLogicalVolume();
    G4VPhysicalVolume* vessel_internal_phys = vessel_->GetInternalPhysicalVolume();
    G4ThreeVector vessel_displacement = shielding_->GetAirDisplacement(); // explained below
    gate_zpos_in_vessel_ = vessel_->GetELzCoord();

    // SHIELDING
    shielding_->Construct();
    shielding_->SetELzCoord(gate_zpos_in_vessel_);
    G4LogicalVolume* shielding_logic     = shielding_->GetLogicalVolume();
    G4LogicalVolume* shielding_air_logic = shielding_->GetAirLogicalVolume();

    // Recall that airbox is slighly displaced in Y dimension. In order to avoid
    // mistmatch with vertex generators, we place the vessel in the center of the world volume
    new G4PVPlacement(0, -vessel_displacement, vessel_logic,
                      "VESSEL", shielding_air_logic, false, 0);

    // INNER ELEMENTS
    inner_elements_->SetLogicalVolume(vessel_internal_logic);
    inner_elements_->SetPhysicalVolume(vessel_internal_phys);
    inner_elements_->SetELzCoord(gate_zpos_in_vessel_);
    inner_elements_->SetELtoSapphireWDWdistance(gate_sapphire_wdw_distance_);
    inner_elements_->SetELtoTPdistance         (gate_tracking_plane_distance_);
    inner_elements_->Construct();

    // INNER COPPER SHIELDING
    ics_->SetLogicalVolume(vessel_internal_logic);
    ics_->SetELzCoord(gate_zpos_in_vessel_);
    ics_->SetELtoSapphireWDWdistance(gate_sapphire_wdw_distance_);
    ics_->SetELtoTPdistance         (gate_tracking_plane_distance_);
    ics_->SetPortZpositions(vessel_->GetPortZpositions());
    ics_->Construct();

    G4ThreeVector gate_pos(0., 0., -gate_zpos_in_vessel_);
    if (lab_walls_){
      G4ThreeVector castle_pos(0., hallA_walls_->GetLSCHallACastleY(),
                               hallA_walls_->GetLSCHallACastleZ());

      new G4PVPlacement(0, castle_pos, shielding_logic,
                        "LEAD_BOX", hallA_logic_, false, 0);
      new G4PVPlacement(0, gate_pos - castle_pos, hallA_logic_,
                        "Hall_A", lab_logic_, false, 0, false);
    }
    else {
      new G4PVPlacement(0, gate_pos, shielding_logic, "LEAD_BOX", lab_logic_, false, 0);
    }

    //// VERTEX GENERATORS
    lab_gen_ =
      new BoxPointSampler(lab_size_ - 1.*m, lab_size_ - 1.*m, lab_size_  - 1.*m, 1.*m,
                          G4ThreeVector(0., 0., 0.), 0);
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
             (region == "INNER_AIR") ||
             (region == "EXTERNAL") ||
             (region == "SHIELDING_STRUCT") ||
             (region == "PEDESTAL") ||
             (region == "BUBBLE_SEAL") ||
             (region == "EDPM_SEAL")) {
      vertex = shielding_->GenerateVertex(region);
    }

    // Vessel regions
    else if ((region == "VESSEL")  ||
             (region == "PORT_1a") ||
             (region == "PORT_2a") ||
             (region == "PORT_1b") ||
             (region == "PORT_2b")) {
      vertex = vessel_->GenerateVertex(region);
    }

    // Inner copper shielding
    else if (region == "ICS"){
      vertex = ics_->GenerateVertex(region);
    }

    // Inner elements (photosensors' planes and field cage)
    else if ((region == "CENTER") ||
             (region == "ACTIVE") ||
             (region == "CATHODE_RING") ||
             (region == "BUFFER") ||
             (region == "XENON") ||
             (region == "LIGHT_TUBE") ||
             (region == "HDPE_TUBE") ||
             (region == "EL_GAP") ||
             (region == "EP_COPPER_PLATE") ||
             (region == "SAPPHIRE_WINDOW") ||
             (region == "OPTICAL_PAD") ||
             (region == "PMT_BODY") ||
             (region == "PMT") ||
             (region == "PMT_BASE") ||
             (region == "TP_COPPER_PLATE") ||
             (region == "SIPM_BOARD") ||
             (region == "DB_PLUG") ||
             (region == "EL_TABLE") ||
             (region == "FIELD_RING") ||
             (region == "GATE_RING") ||
             (region == "ANODE_RING") ||
             (region == "RING_HOLDER")) {
      vertex = inner_elements_->GenerateVertex(region);
    }

    else if (region == "AD_HOC") {
      // AD_HOC does not need to be shifted because it is passed by the user
      vertex = specific_vertex_;
      return vertex;
    }

    // Lab walls
    else if ((region == "HALLA_INNER") || (region == "HALLA_OUTER")){
      if (!lab_walls_)
        G4Exception("[Next100]", "GenerateVertex()", FatalException,
                    "This vertex generation region must be used with lab_walls == true!");
      vertex = hallA_walls_->GenerateVertex(region);
      while (vertex[1]<(-shielding_->GetHeight()/2.)){
        vertex = hallA_walls_->GenerateVertex(region);}
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
