// -----------------------------------------------------------------------------
// nexus | NextNew.cc
//
// Main class that builds the NEXT-WHITE detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextNew.h"

#include "LSCHallA.h"
#include "Next100Shielding.h"
#include "NextNewPedestal.h"
#include "NextNewMiniCastle.h"
#include "NextNewVessel.h"
#include "NextNewIcs.h"
#include "NextNewInnerElements.h"
#include "DiskSource.h"
#include "Th228Source.h"
#include "Na22Source.h"
#include "CylinderPointSampler.h"
#include "BoxPointSampler.h"
#include "Visibilities.h"
#include "CalibrationSource.h"
#include "NaIScintillator.h"
#include "SurroundingAir.h"
#include "LeadCollimator.h"
#include "CollProtection.h"
#include "CollSupport.h"
#include "ExtraVessel.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4RotationMatrix.hh>
#include <G4Transform3D.hh>
#include <G4SDManager.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextNew::NextNew():
    BaseGeometry(),
    // Lab dimensions
    lab_size_ (5. * m),
    rot_angle_(pi),
    lead_block_(false),
    lead_dist_(0.*mm),
    ext_scint_(false),
    calib_port_(""),
    dist_scint_(25.*cm),
    lead_castle_(true),
    lab_walls_(false),
    disk_source_(false),
    source_mat_(""),
    source_dist_from_anode_(15.*cm),
    pedestal_pos_(-568.*mm)
    //   ext_source_distance_(0.*mm)
    // Buffer gas dimensions
  {
    //Lab walls
    hallA_walls_ = new LSCHallA();
    //Shielding
    shielding_ = new Next100Shielding();
    //Pedestal
    pedestal_ = new NextNewPedestal();
    // Mini lead castle
    mini_castle_ = new NextNewMiniCastle();

    //Vessel
    vessel_ = new NextNewVessel();
    //ICS
    ics_ = new NextNewIcs();
    //Inner elements
    inner_elements_ = new NextNewInnerElements();

    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    msg_->DeclareProperty("lead_block", lead_block_, "Block of lead on the lateral port");

    G4GenericMessenger::Command& lead_dist_cmd =
      msg_->DeclareProperty("lead_distance", lead_dist_,
                          "Distance between the two blocks of lead");
    lead_dist_cmd.SetUnitCategory("Length");
    lead_dist_cmd.SetParameterName("lead_distance", false);
    lead_dist_cmd.SetRange("lead_distance>=0.");

    msg_->DeclareProperty("ext_scint", ext_scint_, "Placement of external NaI scintillator");
    msg_->DeclareProperty("calib_port", calib_port_, "Where calibration source is placed (lateral/axial/upper)");

    G4GenericMessenger::Command& scint_dist_cmd =
      msg_->DeclareProperty("scint_distance", dist_scint_,
			    "Distance between the end of the port tube and the NaI scintillator");
    scint_dist_cmd.SetUnitCategory("Length");
    scint_dist_cmd.SetParameterName("scint_distance", false);
    scint_dist_cmd.SetRange("scint_distance>=0.");

    msg_->DeclareProperty("lead_castle", lead_castle_, "Placement of lead castle");
    msg_->DeclareProperty("lab_walls", lab_walls_, "Placement of Hall A walls");
    msg_->DeclareProperty("disk_source", disk_source_, "External disk-shape calibration source");
    msg_->DeclareProperty("source_material", source_mat_, "Kind of external disk-shape calibration source");

    G4GenericMessenger::Command& source_dist_cmd =
      msg_->DeclareProperty("distance_from_anode", source_dist_from_anode_,
			    "Distance of source from anode");
    source_dist_cmd.SetUnitCategory("Length");
    source_dist_cmd.SetParameterName("distance_from_anode", false);
    source_dist_cmd.SetRange("distance_from_anode>=0.");

    cal_ = new CalibrationSource();

    naI_ = new NaIScintillator();

    air_ = new SurroundingAir();

    coll_ = new LeadCollimator();

    extra_ = new ExtraVessel();

  }

  NextNew::~NextNew()
  {
    //deletes
    delete hallA_walls_;
    delete shielding_;
    delete pedestal_;
    delete mini_castle_;

    delete vessel_;
    delete ics_;
    delete inner_elements_;

    delete lab_gen_;
    delete lat_source_gen_;
    delete axial_source_gen_;
  }

  void NextNew::BuildExtScintillator(G4ThreeVector pos, const G4RotationMatrix& rot)
  {
  naI_->Construct();
  G4LogicalVolume* sc_logic = naI_->GetLogicalVolume();

  G4ThreeVector pos_scint =
    G4ThreeVector(pos.getX(), pos.getY(), pos.getZ());
  new G4PVPlacement(G4Transform3D(rot, pos_scint), sc_logic, "NaI",
		    air_logic_, false, 0, false);
  }

  void NextNew::Construct()
  {
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air surrounding the detector so that events
    //(from calibration sources or cosmic rays) can be generated on the outside.
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
      
      lab_logic_ = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    }
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);

    // Set this volume as the wrapper for the whole geometry
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(lab_logic_);

    G4LogicalVolume* surroundings_logic;
    G4String         surroundings_name;
    if (lead_castle_) {
      //SHIELDING
      shielding_->Construct();
      G4LogicalVolume*
      shielding_logic = shielding_->GetLogicalVolume();
      // shielding_air_logic_ = shielding_->GetAirLogicalVolume();
      air_logic_ = shielding_->GetAirLogicalVolume();

      surroundings_logic = shielding_logic;
      surroundings_name  = "LEAD_BOX";

    } else {
      air_->Construct();
      //shieldingair__logic_ = air_->GetLogicalVolume();
      air_logic_ = air_->GetLogicalVolume();

      surroundings_logic = air_logic_;
      surroundings_name  = "AIR";
    }

    //VESSEL
    vessel_->Construct();
    G4LogicalVolume* vessel_logic = vessel_->GetLogicalVolume();
    G4ThreeVector position(0.,0.,0.);
    new G4PVPlacement(0, position, vessel_logic,
		      "VESSEL", air_logic_, false, 0, false);
    G4LogicalVolume* vessel_gas_logic = vessel_->GetInternalLogicalVolume();
    G4VPhysicalVolume* vessel_gas_phys = vessel_->GetInternalPhysicalVolume();

     //INNER ELEMENTS
    inner_elements_->SetMotherLogicalVolume(vessel_gas_logic);
    inner_elements_->SetMotherPhysicalVolume(vessel_gas_phys);
    inner_elements_->Construct();
    shielding_->SetELzCoord(inner_elements_->GetELzCoord());
    vessel_->SetELzCoord(inner_elements_->GetELzCoord());

    displ_ = G4ThreeVector(0., 0., inner_elements_->GetELzCoord());
    G4RotationMatrix rot;
    rot.rotateY(rot_angle_);
    if (lab_walls_){
      G4ThreeVector castle_pos(0., hallA_walls_->GetLSCHallACastleY(),
			       hallA_walls_->GetLSCHallACastleZ());
      new G4PVPlacement(G4Transform3D(rot, castle_pos),
       			surroundings_logic, surroundings_name,
       			hallA_logic_, false, 0, false);
      new G4PVPlacement(0, displ_ - castle_pos, hallA_logic_, "Hall_A",
      			lab_logic_, false, 0, false);
    } else {
      new G4PVPlacement(G4Transform3D(rot, displ_),
			surroundings_logic, surroundings_name,
			lab_logic_, false, 0, false);
    }

     //ICS
    ics_->SetLogicalVolume(vessel_gas_logic);
    ics_->SetNozzlesZPosition(vessel_->GetLATNozzleZPosition(), vessel_->GetUPNozzleZPosition());
    ics_->SetELzCoord(inner_elements_->GetELzCoord());
    ics_->Construct();

    //ELEMENTS BEHIND THE TRACKING PLANE, OUTSIDE THE VESSEL
    extra_->Construct();
    G4LogicalVolume* extra_logic = extra_->GetLogicalVolume();

    extra_rot_ = new G4RotationMatrix();
    extra_rot_->rotateX(pi/2);
    extra_pos_ = G4ThreeVector(0., 15.*cm, vessel_->GetLength()/2. + 40.*cm);
    new G4PVPlacement(G4Transform3D(*extra_rot_, extra_pos_), extra_logic,
		      "EXTRA_VESSEL", air_logic_, false, 0, false);

    //PEDESTAL
    pedestal_->SetLogicalVolume(air_logic_);
    pedestal_->SetPosition(pedestal_pos_);
    pedestal_->Construct();

    //MINI LEAD CASTLE
    mini_castle_->SetLogicalVolume(air_logic_);
    mini_castle_->SetELzCoord(inner_elements_->GetELzCoord());
    G4double ped_y_size = pedestal_->GetDimensions().y();
    mini_castle_->SetPedestalSurfacePosition(pedestal_pos_ + ped_y_size/2.);
    mini_castle_->Construct();


    G4ThreeVector lat_pos = vessel_->GetLatExtSourcePosition(); // this is the position of the end of the port tube
    G4RotationMatrix* lat_rot = new G4RotationMatrix();
    lat_rot->rotateY(-pi/2);

    G4ThreeVector up_pos = vessel_->GetUpExtSourcePosition(); // this is the position of the end of the port tube
    G4RotationMatrix* up_rot = new G4RotationMatrix();
    up_rot->rotateX(pi/2.);

    G4ThreeVector axial_pos = vessel_->GetAxialExtSourcePosition(); // this is the position of the end of the port tube
    G4RotationMatrix* ax_rot = new G4RotationMatrix();
    ax_rot->rotateY(2*pi);


    if (disk_source_) {
      if (source_mat_ == "Na") {
        source_ = new Na22Source();
      } else if (source_mat_ == "Th") {
        source_ = new Th228Source();
      } else {
        G4Exception("[NextNew]", "Construct()", FatalException,
                    "The material of disk source must be Na or Th!");
      }

      source_->Construct();
      G4LogicalVolume* source_logic = source_->GetLogicalVolume();

      // This is the position of the whole source + plastic support.
      if (calib_port_ == "lateral") {
        G4ThreeVector lat_pos_source = G4ThreeVector(lat_pos.getX() + source_->GetSupportThickness()/2., lat_pos.getY(), lat_pos.getZ());

        new G4PVPlacement(G4Transform3D(*lat_rot, lat_pos_source), source_logic, "SOURCE",
                          air_logic_, false, 0, false);
      } else if (calib_port_ == "upper") {
        G4ThreeVector up_pos_source =
          G4ThreeVector(up_pos.getX() , up_pos.getY() + source_->GetSupportThickness()/2., up_pos.getZ());

        new G4PVPlacement(G4Transform3D(*up_rot, up_pos_source), source_logic, "SOURCE",
                          air_logic_, false, 0, false);
      } else if (calib_port_ == "random") {
        G4ThreeVector random_pos_source =
          G4ThreeVector(up_pos.getX(), vessel_->GetOuterRadius() + source_->GetSupportThickness()/2., inner_elements_->GetELzCoord() - source_dist_from_anode_);
        new G4PVPlacement(G4Transform3D(*up_rot, random_pos_source), source_logic, "SOURCE",
                          air_logic_, false, 0, true);
      } else {
        G4Exception("[NextNew]", "Construct()", FatalException,
                    "The placement of disk source must be lateral or upper!");
      }

      G4VisAttributes light_brown_col = nexus::CopperBrown();
      source_logic->SetVisAttributes(light_brown_col);
    }

    // Build NaI external scintillator

    if (ext_scint_) {
      if (calib_port_ == "lateral") {
        // G4RotationMatrix rot;
        // rot.rotateY(-pi/2.);
        BuildExtScintillator(G4ThreeVector(lat_pos.getX() + naI_->GetLength()/2. + dist_scint_,
                                           lat_pos.getY(), lat_pos.getZ()), *lat_rot);
      } else if (calib_port_ == "axial") {
        //    G4RotationMatrix rot_ax;
        BuildExtScintillator(G4ThreeVector(axial_pos.getX(), axial_pos.getY(),
                                           axial_pos.getZ() - dist_scint_ - naI_->GetLength()/2.), *ax_rot);
      } else {
         G4Exception("[NextNew]", "Construct()", FatalException,
		  "The placement of external scintillator must be lateral or axial!");
      }
    }

    G4ThreeVector source_pos;

    cal_->Construct();

    if (lead_block_) {

      coll_->Construct();
      G4LogicalVolume* coll_logic = coll_->GetLogicalVolume();
      G4double coll_centre = coll_->GetAxisCentre();

      CollProtection coll_protection;
      coll_protection.Construct();
      G4LogicalVolume* coll_protection_logic = coll_protection.GetLogicalVolume();

      CollSupport coll_support;
      coll_support.Construct();
      G4LogicalVolume* coll_support_logic = coll_support.GetLogicalVolume();

      if (calib_port_ == "lateral") {

        G4double vessel_out_diam = 664*mm;
        G4ThreeVector pos(vessel_out_diam/2. + coll_centre, lat_pos.getY(), lat_pos.getZ());
        new G4PVPlacement(G4Transform3D(*lat_rot, pos), coll_logic, "LEAD_COLLIMATOR",
                          air_logic_, false, 0, false);

        G4ThreeVector pos_protection(vessel_out_diam/2. + coll_->GetLength() + coll_protection.GetAxisCentre(),
                                     lat_pos.getY(), lat_pos.getZ());
        new G4PVPlacement(G4Transform3D(*lat_rot, pos_protection), coll_protection_logic, "SOURCE_PROTECTION",
                          air_logic_, false, 0, false);

        G4LogicalVolume* cal_logic = cal_->GetLogicalVolume();
        source_pos = G4ThreeVector(vessel_out_diam/2. + coll_->GetLength() - cal_->GetCapsuleThickness()/2.,
                                   lat_pos.getY(), lat_pos.getZ());
        new G4PVPlacement(G4Transform3D(*lat_rot, source_pos), cal_logic,
                          "SCREW_SUPPORT", air_logic_, false, 0, false);

      } else if (calib_port_ == "axial") {

        G4ThreeVector pos(axial_pos.getX(), axial_pos.getY(), axial_pos.getZ() - coll_centre);
        // new G4PVPlacement(0, pos, coll_logic, "LEAD_COLLIMATOR",
        //                   air_logic_, false, 0, true);
        G4ThreeVector pos_protection(axial_pos.getX(), axial_pos.getY(),
                                     axial_pos.getZ() - coll_->GetLength() - coll_protection.GetAxisCentre());
        new G4PVPlacement(0, pos_protection, coll_protection_logic, "SOURCE_PROTECTION",
                          air_logic_, false, 0, false);
        G4ThreeVector pos_support(axial_pos.getX(), axial_pos.getY() - coll_support.GetYDisplacement(),
                                     axial_pos.getZ() + coll_support.GetAxisCentre());
        new G4PVPlacement(0, pos_support, coll_support_logic, "SOURCE_SUPPORT",
                          air_logic_, false, 0, false);

        G4LogicalVolume* cal_logic = cal_->GetLogicalVolume();
        source_pos = G4ThreeVector(axial_pos.getX(), axial_pos.getY(),
                                   axial_pos.getZ() - coll_->GetLength() + cal_->GetCapsuleThickness()/2.);

        new G4PVPlacement(G4Transform3D(*ax_rot, source_pos), cal_logic,
                          "SCREW_SUPPORT", air_logic_, false, 0, false);

      } else {
        G4Exception("[NextNew]", "Construct()", FatalException,
                    "The placement of lead collimator must be lateral or axial!");
      }

    }

    //// VERTEX GENERATORS   //
    lab_gen_ =
      new BoxPointSampler(lab_size_ - 1.*m, lab_size_ - 1.*m, lab_size_ - 1.*m, 1.*m,G4ThreeVector(0.,0.,0.),0);

    // These are the positions of the source inside the capsule
    G4ThreeVector gen_pos_lat = source_pos - G4ThreeVector(cal_->GetSourceZpos(), 0., 0.);
    G4ThreeVector gen_pos_axial = source_pos + G4ThreeVector(0, 0., cal_->GetSourceZpos());
    lat_source_gen_ = new CylinderPointSampler(0., cal_->GetSourceThickness(), cal_->GetSourceDiameter()/2.,
                                               0., gen_pos_lat, lat_rot);

    axial_source_gen_ = new CylinderPointSampler(0., cal_->GetSourceThickness(), cal_->GetSourceDiameter()/2.,
                                               0., gen_pos_axial, ax_rot);

    // These are the vertices of the external disk source
    if (disk_source_) {
      G4double source_diam = source_->GetSourceDiameter();
      G4double source_thick = source_->GetSourceThickness();
      G4ThreeVector lat_pos_gen =
        G4ThreeVector(lat_pos.getX() + source_->GetSourceThickness()/2., lat_pos.getY(), lat_pos.getZ());
      G4ThreeVector up_pos_gen =
        G4ThreeVector(up_pos.getX(), up_pos.getY() + source_->GetSourceThickness()/2., up_pos.getZ());
       G4ThreeVector random_pos_gen =
        G4ThreeVector(up_pos.getX(), vessel_->GetOuterRadius() + source_->GetSourceThickness()/2., inner_elements_->GetELzCoord() - source_dist_from_anode_);
      source_gen_lat_ = new CylinderPointSampler(0., source_thick, source_diam/2., 0., lat_pos_gen, lat_rot);
      source_gen_up_ = new CylinderPointSampler(0., source_thick, source_diam/2., 0., up_pos_gen, up_rot);
      source_gen_random_ = new CylinderPointSampler(0., source_thick, source_diam/2., 0., random_pos_gen, up_rot);
    }

  }



  G4ThreeVector NextNew::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    //AIR AROUND SHIELDING
    if (region == "LAB") {
      vertex = lab_gen_->GenerateVertex("INSIDE");
    }
    /// Calibration source in capsule, placed inside Jordi's lead,
    /// at the end (lateral and axial ports).
    else if (region == "EXTERNAL_PORT_ANODE") {
      if (lead_block_) {
        vertex =  lat_source_gen_->GenerateVertex("BODY_VOL");
      } else {
        G4Exception("[NextNew]", "GenerateVertex()", FatalException,
                    "This vertex generation region must be used together with lead_block == true!");
      }
    }
    else if (region == "EXTERNAL_PORT_AXIAL") {
      if (lead_block_) {
        vertex =  axial_source_gen_->GenerateVertex("BODY_VOL");
      } else {
        G4Exception("[NextNew]", "GenerateVertex()", FatalException,
                    "This vertex generation region must be used together with lead_block == true!");
      }
    }
    // Vertex just outside the axial port
    else if (region == "SOURCE_PORT_AXIAL_EXT") {
      vertex = vessel_->GetAxialExtSourcePosition();
    }
    // Vertex just outside the lateral port
    else if (region == "SOURCE_PORT_LATERAL_EXT") {
      vertex = vessel_->GetLatExtSourcePosition();
    }
    // Extended sources with the shape of a disk outside port
    else if (region == "SOURCE_PORT_LATERAL_DISK") {
      vertex =  source_gen_lat_->GenerateVertex("BODY_VOL");
    }
    else if (region == "SOURCE_PORT_UP_DISK") {
      vertex =  source_gen_up_->GenerateVertex("BODY_VOL");
    }
     else if (region == "SOURCE_DISK") {
      vertex =  source_gen_random_->GenerateVertex("BODY_VOL");
    }
    else if ( (region == "SHIELDING_LEAD") || (region == "SHIELDING_STEEL") ||
	          (region == "INNER_AIR")  || (region == "SHIELDING_STRUCT") ||
	          (region == "EXTERNAL") ) {
      vertex = shielding_->GenerateVertex(region);
    }
    //PEDESTAL
    else if (region == "PEDESTAL") {
      vertex = pedestal_->GenerateVertex(region);
    }
    // EXTRA ELEMENTS
    else if (region == "EXTRA_VESSEL") {
      G4ThreeVector ini_vertex = extra_->GenerateVertex(region);
      ini_vertex.rotate(pi/2., G4ThreeVector(1., 0., 0.));
      vertex = ini_vertex + extra_pos_;
    }
    // Lab walls
    else if ((region == "HALLA_INNER") || (region == "HALLA_OUTER")){
      if (!lab_walls_)
        G4Exception("[NextNew]", "GenerateVertex()", FatalException,
                    "This vertex generation region must be used with lab_walls == true!");
      vertex = hallA_walls_->GenerateVertex(region);
      vertex = displ_ + vertex;
    }

    //  MINI CASTLE and RADON
    // on the inner lead surface (SHIELDING_GAS) and on the outer mini lead castle surface (RN_MINI_CASTLE)
    else if ((region == "MINI_CASTLE") ||
             (region == "RN_MINI_CASTLE") ||
             (region == "MINI_CASTLE_STEEL")) {
      vertex = mini_castle_->GenerateVertex(region);
    }
    //VESSEL REGIONS
    else if ((region == "VESSEL") ||
             (region == "SOURCE_PORT_ANODE") ||
             (region == "SOURCE_PORT_UP") ||
             (region == "SOURCE_PORT_AXIAL") ||
             (region == "INTERNAL_PORT_ANODE") ||
             (region == "INTERNAL_PORT_UPPER") ||
             (region == "INTERNAL_PORT_AXIAL")){
      vertex = vessel_->GenerateVertex(region);
    }
    // ICS REGIONS
    else if (region == "ICS") {
      vertex = ics_->GenerateVertex(region);
    }
    //INNER ELEMENTS
    else if ((region == "CENTER") ||
             (region == "CARRIER_PLATE") ||
             (region == "ENCLOSURE_BODY") ||
             (region == "ENCLOSURE_WINDOW") ||
             (region == "OPTICAL_PAD") ||
             (region == "PMT_BODY") ||
             (region == "PMT_BASE") ||
             (region == "INT_ENCLOSURE_SURF") ||
             (region == "PMT_SURF") ||
             (region == "DRIFT_TUBE") ||
             (region == "ANODE_QUARTZ")||
             (region == "HDPE_TUBE") ||
             (region == "XENON") ||
             (region == "ACTIVE") ||
             (region == "BUFFER") ||
             (region == "EL_GAP") ||
             (region == "EL_TABLE") ||
             (region == "AD_HOC") ||
             (region == "CATHODE")||
             (region == "TRACKING_FRAMES") ||
             (region == "SUPPORT_PLATE") ||
             (region == "DICE_BOARD") ||
             (region == "DB_PLUG")) {
      vertex = inner_elements_->GenerateVertex(region);
    }
    else {
      G4Exception("[NextNew]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    // AD_HOC is not rotated and shifted because it is passed by the user
    // The LSC HallA vertices are already corrected so no need.
    if ((region == "AD_HOC") ||
        (region == "HALLA_OUTER") ||
        (region == "HALLA_INNER"))
      return vertex;

    // In EL_GAP, x and y coordinates are passed by the user,
    // but the z coordinate is not. Therefore, rotation + displacement
    // must be applied to get the correct z, but x and y must be left
    // unchanged.
    if (region == "EL_GAP") {
      // First rotate, then shift to return the correct z coordinate
      vertex.rotate(rot_angle_, G4ThreeVector(0., 1., 0.));
      vertex = vertex + displ_;

      // Change back x coordinate alone (y is not touched).
      vertex.setX(-vertex.x());

      return vertex;
    }

    // First rotate, then shift
    vertex.rotate(rot_angle_, G4ThreeVector(0., 1., 0.));
    vertex = vertex + displ_;

    return vertex;
  }


} //end namespace nexus
