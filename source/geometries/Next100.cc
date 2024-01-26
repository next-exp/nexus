// ----------------------------------------------------------------------------
// nexus | Next100.cc
//
// Main class that constructs the geometry of the NEXT-100 detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100.h"
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

    // common variables used in geometry components
    grid_thickness_ (0.13 * mm),
    //ep_fc_distance_ (24.8 * mm), /// this value was measured by Sara in the step file
    gate_tracking_plane_distance_(25. * mm + grid_thickness_), // Jordi = 1.5 (distance TP plate-anode ring) + 13.5 (anode ring thickness) + 10 (EL gap)
    gate_sapphire_wdw_distance_  (1458.8 * mm - grid_thickness_), // Jordi
    ics_ep_lip_width_ (55. * mm), // length of the step cut out in the ICS, in the EP side
    fc_displ_x_ (-3.7 * mm), // displacement of the field cage volumes from 0
    fc_displ_y_ (-6.4 * mm), // displacement of the field cage volumes from 0

    specific_vertex_{},
    lab_walls_(false),
    print_(false)
  {

    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");

    msg_->DeclarePropertyWithUnit("specific_vertex", "mm",  specific_vertex_,
      "Set generation vertex.");

    msg_->DeclareProperty("lab_walls", lab_walls_, "Placement of Hall A walls.");

    msg_->DeclareProperty("print_sns_pos", print_, "Print sensor positions.");

  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones

  // Shielding
  shielding_ = new Next100Shielding();

  //Lab walls
  hallA_walls_ = new LSCHallA();

  // Vessel
  vessel_ = new Next100Vessel(ics_ep_lip_width_);

  // Internal copper shielding
  ics_ = new Next100Ics(ics_ep_lip_width_);

  // Inner Elements
  inner_elements_ = new Next100InnerElements(grid_thickness_);

  }


  Next100::~Next100()
  {
    delete inner_elements_;
    delete ics_;
    delete vessel_;
    delete shielding_;
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
      G4Box* lab_solid =
        new G4Box("LAB", hallA_length, hallA_length, hallA_length);
      G4Material* vacuum =
        G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
      lab_logic_ = new G4LogicalVolume(lab_solid, vacuum, "LAB");
      this->SetSpan(2 * hallA_length);
    }
    else {
      G4Box* lab_solid =
        new G4Box("LAB", lab_size_/2., lab_size_/2., lab_size_/2.);
      lab_logic_ = new G4LogicalVolume(lab_solid,
        G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    }
    lab_logic_->SetVisAttributes(G4VisAttributes::GetInvisible());

    // Set this volume as the wrapper for the whole geometry
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(lab_logic_);

    // VESSEL (initialize first since it defines EL position)
    // The z coord origin is not set, because it is not defined, yet
    vessel_->SetELtoTPdistance(gate_tracking_plane_distance_);
    vessel_->SetCoordOrigin(G4ThreeVector(fc_displ_x_,
                                          fc_displ_y_,
                                          0.));
    vessel_->Construct();
    G4LogicalVolume* vessel_logic = vessel_->GetLogicalVolume();
    G4LogicalVolume* vessel_internal_logic  =
      vessel_->GetInternalLogicalVolume();
    G4VPhysicalVolume* vessel_internal_phys =
      vessel_->GetInternalPhysicalVolume();
    G4ThreeVector vessel_displacement =
      shielding_->GetAirDisplacement(); // explained below

    coord_origin_ = G4ThreeVector(fc_displ_x_, fc_displ_y_, vessel_->GetGateZpos());

    // SHIELDING
    shielding_->SetCoordOrigin(coord_origin_);
    shielding_->Construct();
    G4LogicalVolume* shielding_logic     = shielding_->GetLogicalVolume();
    G4LogicalVolume* shielding_air_logic = shielding_->GetAirLogicalVolume();

    // Recall that air box is slightly displaced in Y dimension.
    // In order to avoid mistmatch with vertex generators,
    // we place the vessel in the center of the world volume
    new G4PVPlacement(0, -vessel_displacement, vessel_logic,
                      "VESSEL", shielding_air_logic, false, 0);

    // INNER ELEMENTS
    inner_elements_->SetLogicalVolume(vessel_internal_logic);
    inner_elements_->SetPhysicalVolume(vessel_internal_phys);
    inner_elements_->SetCoordOrigin(coord_origin_);
    inner_elements_->SetELtoSapphireWDWdistance(gate_sapphire_wdw_distance_);
    inner_elements_->SetELtoTPdistance         (gate_tracking_plane_distance_);
    inner_elements_->Construct();

    // INNER COPPER SHIELDING
    ics_->SetLogicalVolume(vessel_internal_logic);
    ics_->SetCoordOrigin(coord_origin_);
    ics_->SetELtoSapphireWDWdistance(gate_sapphire_wdw_distance_);
    ics_->SetELtoTPdistance         (gate_tracking_plane_distance_);
    ics_->SetPortZpositions(vessel_->GetPortZpositions());
    ics_->Construct();

    if (lab_walls_){
      G4ThreeVector castle_pos(0., hallA_walls_->GetLSCHallACastleY(),
                               hallA_walls_->GetLSCHallACastleZ());

      new G4PVPlacement(0, castle_pos, shielding_logic,
                        "LEAD_BOX", hallA_logic_, false, 0);
      new G4PVPlacement(0, -coord_origin_ - castle_pos, hallA_logic_,
                        "Hall_A", lab_logic_, false, 0, false);
    }
    else {
      new G4PVPlacement(0, -coord_origin_, shielding_logic,
                        "LEAD_BOX", lab_logic_, false, 0);
    }

    if (print_) {
      std::vector<G4ThreeVector> pmt_pos  = inner_elements_->GetPMTPosInGas();
      for (unsigned int i=0; i<pmt_pos.size(); i++) {
        G4ThreeVector pos = pmt_pos[i] - coord_origin_;
        G4cout << "PMT " << i << ": " << pos.x() << ", "<< pos.y() << G4endl;
      }

      std::vector<G4ThreeVector> sipm_pos = inner_elements_->GetSiPMPosInGas();
      G4int n_sipm = 0;
      G4int b = 1;
      for (unsigned int i=0; i<sipm_pos.size(); i++) {
        G4ThreeVector pos = sipm_pos[i] - coord_origin_;
        G4int id = 1000 * b + n_sipm;
        G4cout << "SiPM " << id << ": " << pos.x() << ", "<< pos.y() << G4endl;
        n_sipm++;
        if (id % 1000 == 63) {
          n_sipm = 0;
          b++;
        }
      }
    }
  }


  G4ThreeVector Next100::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // Shielding regions
    if ((region == "SHIELDING_LEAD")  ||
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

    vertex = vertex - coord_origin_;

    return vertex;
  }


  G4ThreeVector Next100::ProjectToRegion(const G4String& region,
					 const G4ThreeVector& point,
					 const G4ThreeVector& dir) const
  {
    // Project along dir from point to find the first intersection
    // with region.
    G4ThreeVector vertex(0., 0., 0.);
    if ((region == "HALLA_OUTER") || (region == "HALLA_INNER")){
      if (!lab_walls_)
	G4Exception("[Next100]", "ProjectToRegion()", FatalException,
                    "To project to this region you need lab_walls == true!");
      return hallA_walls_->ProjectToRegion(region, point, dir);
    }
    else {
      G4Exception("[Next100]", "ProjectToRegion()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex - coord_origin_;
  }

} //end namespace nexus
