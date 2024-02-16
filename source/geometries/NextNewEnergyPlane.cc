// -----------------------------------------------------------------------------
// nexus | NextNewEnergyPlane.cc
//
// Energy plane of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextNewEnergyPlane.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "CylinderPointSamplerLegacy.h"

#include <G4GenericMessenger.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <G4VPhysicalVolume.hh>
#include <G4TransportationManager.hh>
#include <Randomize.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextNewEnergyPlane::NextNewEnergyPlane():

    num_PMTs_ (12),
    num_gas_holes_ (12),
    energy_plane_z_pos_ (-38.2 * cm), // it's -347 * mm - 35 mm to match the 130 mm distance between cathode and surface of sapphire windows.
    // between the two parts of the reflector is 1.5 cm
    //energy_plane_z_pos_ (-34.7 * cm),//center to EP surface  //middle_nozzle(43.5)-(right_nozzle(15.8)-EP_to_rigth_nozzle(7) (from drawings)
    // Carrier Plate dimensions
    carrier_plate_front_buffer_thickness_ (5. * mm),
    carrier_plate_front_buffer_diam_ (55. *cm),//(630-2*40)
    carrier_plate_thickness_ (12.0 * cm),
    carrier_plate_diam_ (63.0 * cm), // ???
    enclosure_hole_diam_ (9.3 *cm),
    gas_hole_diam_ (8.0 * mm),
    gas_hole_pos_ (20 * mm),
    tpb_thickness_ (1.*micrometer),
    axial_port_hole_diam_ (5 * mm),
    axial_port_thickn_ (4. * mm),
    visibility_(1)

  {
    /// Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNewEnergyPlane.");
    msg_->DeclareProperty("energy_plane_vis", visibility_, "Energy Plane Visibility");

    /// The NextNewPmtEnclosure
    enclosure_ = new NextNewPmtEnclosure();
    G4double enclosure_z_center = enclosure_->GetObjectCenter().z();
    enclosure_z_pos_ = energy_plane_z_pos_ - carrier_plate_front_buffer_thickness_ - enclosure_z_center;
  }
  void NextNewEnergyPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }

  void NextNewEnergyPlane::Construct()
  {
    GeneratePMTsPositions();
    GenerateGasHolePositions();

    ////  CARRIER PLATE  ////
    G4Tubs* carrier_plate_nh_solid =
      new G4Tubs("CARRIER_NH_PLATE", 0., carrier_plate_diam_/2.,
		 carrier_plate_thickness_/2., 0., twopi);


    //Making front buffer

    G4Tubs* carrier_plate_front_buffer_solid =
      new G4Tubs("CARRIER_PLATE_FBUF_SOLID",0.,
		 carrier_plate_front_buffer_diam_/2.,
		 (carrier_plate_front_buffer_thickness_+1.*mm)/2.,0.,twopi);

    G4SubtractionSolid* carrier_plate_solid =
      new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_nh_solid,
			     carrier_plate_front_buffer_solid, 0,
			     G4ThreeVector(0., 0., carrier_plate_thickness_/2.
					   - carrier_plate_front_buffer_thickness_/2.+.5*mm));

    // Making PMT holes
    G4Tubs* carrier_plate_pmt_hole_solid =
      new G4Tubs("CARRIER_PLATE_PMT_HOLE", 0., enclosure_hole_diam_/2.,
		 (carrier_plate_thickness_+1.*mm)/2., 0., twopi);
    for (int i=0; i<num_PMTs_; i++) {
      carrier_plate_solid =
	new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_solid,
			       carrier_plate_pmt_hole_solid, 0,
			       pmt_positions_[i]);
    }

    //Making holes for XeGas flow
    G4Tubs* gas_hole_solid =
      new G4Tubs("GAS_HOLE", 0., gas_hole_diam_/2,
		 (carrier_plate_thickness_+1*mm)/2., 0., twopi);
    for (int i=0; i<num_gas_holes_; i++){
      carrier_plate_solid =
	new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_solid,
			       gas_hole_solid, 0, gas_hole_positions_[i]);
    }



    // // Material is vacuum
    // G4Material* vacuum =
    //   G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    // vacuum->SetMaterialPropertiesTable(opticalprops::Vacuum());
    //  G4LogicalVolume* axial_port_hole_logic = new G4LogicalVolume(axial_port_hole_solid,
    //			vacuum, "AXIAL_PORT_HOLE");
    //   G4LogicalVolume* axial_port_hole_logic = new G4LogicalVolume(axial_port_hole_solid,
    //			vacuum, "AXIAL_PORT_HOLE");
    // new G4PVPlacement(0,G4ThreeVector(0.,0., - axial_port_thickn_/2.),
    //		    axial_port_hole_logic, "AXIAL_PORT_HOLE",
    //		    carrier_plate_logic, false, 0, true);

    // Making hole for axial port

    G4Tubs* axial_port_hole_solid =
      new G4Tubs("AXIAL_PORT_HOLE", 0., axial_port_hole_diam_/2.,
		 carrier_plate_thickness_/2., 0., twopi);
    carrier_plate_solid  =
      new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_solid, axial_port_hole_solid,
			     0, G4ThreeVector(0., 0., - carrier_plate_front_buffer_thickness_ - axial_port_thickn_));

    G4Material* copper_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    copper_mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* carrier_plate_logic =
      new G4LogicalVolume(carrier_plate_solid,
			  copper_mat,
			  "CARRIER_PLATE");


    ///ENCLOSURES + PMT ///
    enclosure_->Construct();
    G4LogicalVolume* enclosure_logic = enclosure_->GetLogicalVolume();
    G4double enclosure_z_center = enclosure_->GetObjectCenter().z();// return G4ThreeVector(0., 0., enclosure__length/2.);

    ///Placement: the carrier plate skims the plane of the sapphire windows

    // G4double carrier_plate_z_pos =
    //   energy_plane_z_pos_ - carrier_plate_thickness_/2.;
    G4double carrier_plate_z_pos =
      enclosure_z_pos_ + enclosure_z_center - carrier_plate_thickness_/2.;
    //G4cout << "***** " << enclosure_z_pos_ << "  " << carrier_plate_thickness_ << G4endl;

    new G4PVPlacement(0, G4ThreeVector(0.,0.,carrier_plate_z_pos),
		      carrier_plate_logic, "CARRIER_PLATE",
		      mother_logic_, false, 0, false);

    // Placing the enclosures

    G4ThreeVector pos;
    G4ThreeVector tpb_pos;
    for (int i=0; i<num_PMTs_; i++) {
      pos = pmt_positions_[i];
      tpb_pos = pmt_positions_[i];
      pos.setZ(enclosure_z_pos_);
      tpb_pos.setZ(enclosure_z_pos_ + enclosure_z_center + tpb_thickness_/2.);
      new G4PVPlacement(nullptr, pos, enclosure_logic, "ENCLOSURE", mother_logic_,
                        false, i, false);
    }


    /////  SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes brown_col = nexus::CopperBrown();
      brown_col.SetForceSolid(true);
      carrier_plate_logic->SetVisAttributes(brown_col);
      G4VisAttributes green_col = nexus::DarkGreen();
      green_col.SetForceSolid(true);
    } else {
      carrier_plate_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    // VERTEX GENERATORS   //////////
    carrier_gen_ =
      new CylinderPointSamplerLegacy(carrier_plate_diam_/2.,
			       carrier_plate_thickness_, 0., 0.,
			       G4ThreeVector (0., 0., carrier_plate_z_pos));
    // G4double total_vol = carrier_plate_solid->GetCubicVolume();
    //  std::cout<<"CARRIER PLATE (EP) VOLUME: \t"<<total_vol<<std::endl;
  }

  NextNewEnergyPlane::~NextNewEnergyPlane()
  {
    delete carrier_gen_;
  }

  G4ThreeVector NextNewEnergyPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    /// Carrier Plate   // As it is full of holes, let's get sure vertexes are in the right volume
    if (region == "CARRIER_PLATE") {
      G4VPhysicalVolume* VertexVolume;
      do {
	vertex = carrier_gen_->GenerateVertex("INSIDE");
	// To check its volume, you need to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	CalculateGlobalPos(glob_vtx);
	VertexVolume =
	  geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "CARRIER_PLATE");
    }
    //NextNewPmtEnclosures
    else if (region== "ENCLOSURE_BODY"){
      G4ThreeVector ini_vertex = enclosure_->GenerateVertex(region);
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector enclosure_pos = pmt_positions_[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + enclosure_z_pos_);
    }
    else if (region == "ENCLOSURE_WINDOW") {
      G4ThreeVector ini_vertex = enclosure_->GenerateVertex(region);
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector enclosure_pos = pmt_positions_[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + enclosure_z_pos_);
    }
    else if (region == "OPTICAL_PAD") {
      G4ThreeVector ini_vertex = enclosure_->GenerateVertex(region);
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector enclosure_pos = pmt_positions_[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + enclosure_z_pos_);
    }
    else if  (region=="PMT_BASE") {
      G4ThreeVector ini_vertex = enclosure_->GenerateVertex(region);
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector enclosure_pos = pmt_positions_[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + enclosure_z_pos_);
      // std::cout<<"vertx z energy plane  "<< vertex.z()<<std::endl;
      // std::cout<<"vertx x energy plane  "<< vertex.x()<<std::endl;
      // std::cout<<"vertx y energy plane  "<< vertex.y()<<std::endl;
    }
    else if (region== "PMT_BODY"){
      G4ThreeVector ini_vertex = enclosure_->GenerateVertex(region);
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector pmt_pos = pmt_positions_[int(rand)];
      vertex = ini_vertex + pmt_pos;
      vertex.setZ(vertex.z() + enclosure_z_pos_);
    }
    else if (region== "INT_ENCLOSURE_SURF"){
      G4ThreeVector ini_vertex = enclosure_->GenerateVertex(region);
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector enclosure_pos = pmt_positions_[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + enclosure_z_pos_);
    }

    else if (region== "PMT_SURF"){
      G4ThreeVector ini_vertex = enclosure_->GenerateVertex(region);
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector pmt_pos = pmt_positions_[int(rand)];
      vertex = ini_vertex + pmt_pos;
      vertex.setZ(vertex.z() + enclosure_z_pos_);
    }

    else {
      G4Exception("[NextNewEnergyPlane]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }

  void NextNewEnergyPlane::GeneratePMTsPositions()
  {
    /// Function that computes and stores the XY positions of PMTs in the carrier plate
    G4int num_conc_circles = 2;
    G4int num_inner_pmts = 3;
    G4int num_outer_pmts = 9;
    G4double rad1 = 7. * cm;   // inner circle radius
    G4double rad2 = 18.5 * cm;   // outer circle radius
    G4double offset_angle = 20.;
    G4int total_positions = 0;
    G4ThreeVector position(0.,0.,0.);

    for (G4int circle=1; circle <= num_conc_circles; circle++) {
      if(circle==1){
	G4int step_deg = 360.0 / num_inner_pmts;
	for (G4int place=0; place<num_inner_pmts; place++) {
	  G4double angle = offset_angle + (place * step_deg);
	  position.setX(rad1 * sin(angle * deg));
	  position.setY(rad1 * cos(angle * deg));
	  pmt_positions_.push_back(position);
	  total_positions++;
	}
      }
      else if (circle==2){
	G4int step_deg = 360.0 / num_outer_pmts;
	for (G4int place=0; place<num_outer_pmts; place++) {
	  G4double angle = place * step_deg;
	  position.setX(rad2 * sin(angle * deg));
	  position.setY(rad2 * cos(angle * deg));
	  pmt_positions_.push_back(position);
	  total_positions++;
	}
      }
      else { std::cout<<"Error in PMTs positions generation."<<std::endl; }
    }
    // Checking
    if (total_positions != num_PMTs_) {
      G4cout << "\n\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
      exit(0);
    }
  }
  void NextNewEnergyPlane::GenerateGasHolePositions()
  {
    /// Function that computes and stores the XY positions of gas holes in the carrier plate
    G4double rad = carrier_plate_diam_/2. - gas_hole_pos_;
    G4ThreeVector post(0.,0.,0.);
    G4int step_deg = 360.0 /num_gas_holes_;
    for (G4int place=0; place<num_gas_holes_; place++) {
      G4double angle = place * step_deg;
      post.setX(rad * sin(angle * deg));
      post.setY(rad * cos(angle * deg));
      gas_hole_positions_.push_back(post);
    }
  }


}//end namespace nexus
