// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 13 Sept 2013
//
//  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextNewEnergyPlane.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

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
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  NextNewEnergyPlane::NextNewEnergyPlane():

    _num_PMTs (12),
    _num_gas_holes (12),
    _energy_plane_z_pos (-38.2 * cm), // it's -347 * mm - 35 mm to match the 130 mm distance between cathode and surface of sapphire windows.
    // between the two parts of the reflector is 1.5 cm
    //_energy_plane_z_pos (-34.7 * cm),//center to EP surface  //middle_nozzle(43.5)-(right_nozzle(15.8)-EP_to_rigth_nozzle(7) (from drawings)
    // Carrier Plate dimensions
    _carrier_plate_front_buffer_thickness (5. * mm),
    _carrier_plate_front_buffer_diam (55. *cm),//(630-2*40)
    _carrier_plate_thickness (12.0 * cm),
    _carrier_plate_diam (63.0 * cm), // ???
    _enclosure_hole_diam (9.3 *cm),
    _gas_hole_diam (8.0 * mm),
    _gas_hole_pos (20 * mm),
    _tpb_thickness (1.*micrometer),
    _axial_port_hole_diam (5 * mm),
    _axial_port_thickn (4. * mm),
    _visibility(1)

  {
    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNewEnergyPlane.");
    _msg->DeclareProperty("energy_plane_vis", _visibility, "Energy Plane Visibility");

    /// The Enclosure
    _enclosure = new Enclosure();
    G4double enclosure_z_center = _enclosure->GetObjectCenter().z();
    _enclosure_z_pos = _energy_plane_z_pos - _carrier_plate_front_buffer_thickness - enclosure_z_center;
  }
  void NextNewEnergyPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  void NextNewEnergyPlane::Construct()
  {
    GeneratePMTsPositions();
    GenerateGasHolePositions();

    ////  CARRIER PLATE  ////
    G4Tubs* carrier_plate_nh_solid =
      new G4Tubs("CARRIER_NH_PLATE", 0., _carrier_plate_diam/2.,
		 _carrier_plate_thickness/2., 0., twopi);


    //Making front buffer

    G4Tubs* carrier_plate_front_buffer_solid =
      new G4Tubs("CARRIER_PLATE_FBUF_SOLID",0.,
		 _carrier_plate_front_buffer_diam/2.,
		 (_carrier_plate_front_buffer_thickness+1.*mm)/2.,0.,twopi);

    G4SubtractionSolid* carrier_plate_solid =
      new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_nh_solid,
			     carrier_plate_front_buffer_solid, 0,
			     G4ThreeVector(0., 0., _carrier_plate_thickness/2.
					   - _carrier_plate_front_buffer_thickness/2.+.5*mm));

    // Making PMT holes
    G4Tubs* carrier_plate_pmt_hole_solid =
      new G4Tubs("CARRIER_PLATE_PMT_HOLE", 0., _enclosure_hole_diam/2.,
		 (_carrier_plate_thickness+1.*mm)/2., 0., twopi);
    for (int i=0; i<_num_PMTs; i++) {
      carrier_plate_solid =
	new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_solid,
			       carrier_plate_pmt_hole_solid, 0,
			       _pmt_positions[i]);
    }

    //Making holes for XeGas flow
    G4Tubs* gas_hole_solid =
      new G4Tubs("GAS_HOLE", 0., _gas_hole_diam/2,
		 (_carrier_plate_thickness+1*mm)/2., 0., twopi);
    for (int i=0; i<_num_gas_holes; i++){
      carrier_plate_solid =
	new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_solid,
			       gas_hole_solid, 0, _gas_hole_positions[i]);
    }



    // // Material is vacuum
    // G4Material* vacuum =
    //   G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    // vacuum->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());
    //  G4LogicalVolume* axial_port_hole_logic = new G4LogicalVolume(axial_port_hole_solid,
    //			vacuum, "AXIAL_PORT_HOLE");
    //   G4LogicalVolume* axial_port_hole_logic = new G4LogicalVolume(axial_port_hole_solid,
    //			vacuum, "AXIAL_PORT_HOLE");
    // new G4PVPlacement(0,G4ThreeVector(0.,0., - _axial_port_thickn/2.),
    //		    axial_port_hole_logic, "AXIAL_PORT_HOLE",
    //		    carrier_plate_logic, false, 0, true);

    // Making hole for axial port

    G4Tubs* axial_port_hole_solid =
      new G4Tubs("AXIAL_PORT_HOLE", 0., _axial_port_hole_diam/2.,
		 _carrier_plate_thickness/2., 0., twopi);
    carrier_plate_solid  =
      new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_solid, axial_port_hole_solid,
			     0, G4ThreeVector(0., 0., - _carrier_plate_front_buffer_thickness - _axial_port_thickn));


    G4LogicalVolume* carrier_plate_logic =
      new G4LogicalVolume(carrier_plate_solid,
			  G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
			  "CARRIER_PLATE");


    ///ENCLOSURES + PMT ///
    _enclosure->Construct();
    G4LogicalVolume* enclosure_logic = _enclosure->GetLogicalVolume();
    G4double enclosure_z_center = _enclosure->GetObjectCenter().z();// return G4ThreeVector(0., 0., _enclosure_length/2.);

    ///Placement: the carrier plate skims the plane of the sapphire windows

    // G4double carrier_plate_z_pos =
    //   _energy_plane_z_pos - _carrier_plate_thickness/2.;
    G4double carrier_plate_z_pos =
      _enclosure_z_pos + enclosure_z_center - _carrier_plate_thickness/2.;
    //G4cout << "***** " << _enclosure_z_pos << "  " << _carrier_plate_thickness << G4endl;

    new G4PVPlacement(0, G4ThreeVector(0.,0.,carrier_plate_z_pos),
		      carrier_plate_logic, "CARRIER_PLATE",
		      _mother_logic, false, 0, false);


    /// TPB coating on sapphire window
    G4Material* tpb = MaterialsList::TPB();
    tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
    G4double window_diam =  _enclosure->GetWindowDiameter();
    G4Tubs* tpb_solid = new G4Tubs("ENCLOSURE_TPB", 0., window_diam/2,
                                   _tpb_thickness/2., 0., twopi);
    G4LogicalVolume* tpb_logic =
      new G4LogicalVolume(tpb_solid, tpb,"ENCLOSURE_TPB");

    // Optical surface around TPB to model its roughness
    G4OpticalSurface* enclosure_tpb_opsurf =
      new G4OpticalSurface("ENCLOSURE_TPB_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);
    new G4LogicalSkinSurface("ENCLOSURE_TPB_OPSURF", tpb_logic, enclosure_tpb_opsurf);


    G4VisAttributes * visattrib_blue = new G4VisAttributes;
    visattrib_blue->SetColor(0., 0., 1.);
    tpb_logic->SetVisAttributes(visattrib_blue);

    // Placing the enclosures

    G4ThreeVector pos;
    G4ThreeVector tpb_pos;
    for (int i=0; i<_num_PMTs; i++) {
      pos = _pmt_positions[i];
      tpb_pos = _pmt_positions[i];
      pos.setZ(_enclosure_z_pos);
      tpb_pos.setZ(_enclosure_z_pos + enclosure_z_center + _tpb_thickness/2.);
      new G4PVPlacement(0, pos, enclosure_logic, "ENCLOSURE", _mother_logic,
			false, i, false);
      new G4PVPlacement(0, tpb_pos, tpb_logic, "ENCLOSURE_TPB", _mother_logic,
			false, i, false);
      //std::cout<<"enclosure positions"<< _pmt_positions[i]<< _enclosure_z_pos<<std::endl;
    }


    /////  SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes brown_col = nexus::CopperBrown();
      brown_col.SetForceSolid(true);
      carrier_plate_logic->SetVisAttributes(brown_col);
      G4VisAttributes green_col = nexus::DarkGreen();
      green_col.SetForceSolid(true);
      tpb_logic->SetVisAttributes(green_col);
    } else {
      carrier_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    // VERTEX GENERATORS   //////////
    _carrier_gen =
      new CylinderPointSampler(_carrier_plate_diam/2.,
			       _carrier_plate_thickness, 0., 0.,
			       G4ThreeVector (0., 0., carrier_plate_z_pos));
    // G4double total_vol = carrier_plate_solid->GetCubicVolume();
    //  std::cout<<"CARRIER PLATE (EP) VOLUME: \t"<<total_vol<<std::endl;
  }

  NextNewEnergyPlane::~NextNewEnergyPlane()
  {
    delete _carrier_gen;
  }

  G4ThreeVector NextNewEnergyPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    /// Carrier Plate   // As it is full of holes, let's get sure vertexes are in the right volume
    if (region == "CARRIER_PLATE") {
      G4VPhysicalVolume* VertexVolume;
      do {
	vertex = _carrier_gen->GenerateVertex("INSIDE");
	// To check its volume, you need to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	CalculateGlobalPos(glob_vtx);
	VertexVolume =
	  _geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "CARRIER_PLATE");
    }
    //Enclosures
    else if (region== "ENCLOSURE_BODY"){
      G4ThreeVector ini_vertex = _enclosure->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + _enclosure_z_pos);
    }
    else if (region == "ENCLOSURE_WINDOW") {
      G4ThreeVector ini_vertex = _enclosure->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + _enclosure_z_pos);
    }
    else if (region == "OPTICAL_PAD") {
      G4ThreeVector ini_vertex = _enclosure->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + _enclosure_z_pos);
    }
    else if  (region=="PMT_BASE") {
      G4ThreeVector ini_vertex = _enclosure->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + _enclosure_z_pos);
      // std::cout<<"vertx z energy plane  "<< vertex.z()<<std::endl;
      // std::cout<<"vertx x energy plane  "<< vertex.x()<<std::endl;
      // std::cout<<"vertx y energy plane  "<< vertex.y()<<std::endl;
    }
    else if (region== "PMT_BODY"){
      G4ThreeVector ini_vertex = _enclosure->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector pmt_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + pmt_pos;
      vertex.setZ(vertex.z() + _enclosure_z_pos);
    }
    else if (region== "INT_ENCLOSURE_SURF"){
      G4ThreeVector ini_vertex = _enclosure->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + enclosure_pos;
      vertex.setZ(vertex.z() + _enclosure_z_pos);
    }

    else if (region== "PMT_SURF"){
      G4ThreeVector ini_vertex = _enclosure->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector pmt_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + pmt_pos;
      vertex.setZ(vertex.z() + _enclosure_z_pos);
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
	  _pmt_positions.push_back(position);
	  total_positions++;
	}
      }
      else if (circle==2){
	G4int step_deg = 360.0 / num_outer_pmts;
	for (G4int place=0; place<num_outer_pmts; place++) {
	  G4double angle = place * step_deg;
	  position.setX(rad2 * sin(angle * deg));
	  position.setY(rad2 * cos(angle * deg));
	  _pmt_positions.push_back(position);
	  total_positions++;
	}
      }
      else { std::cout<<"Error in PMTs positions generation."<<std::endl; }
    }
    // Checking
    if (total_positions != _num_PMTs) {
      G4cout << "\n\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
      exit(0);
    }
  }
  void NextNewEnergyPlane::GenerateGasHolePositions()
  {
    /// Function that computes and stores the XY positions of gas holes in the carrier plate
    G4double rad = _carrier_plate_diam/2. - _gas_hole_pos;
    G4ThreeVector post(0.,0.,0.);
    G4int step_deg = 360.0 /_num_gas_holes;
    for (G4int place=0; place<_num_gas_holes; place++) {
      G4double angle = place * step_deg;
      post.setX(rad * sin(angle * deg));
      post.setY(rad * cos(angle * deg));
      _gas_hole_positions.push_back(post);
    }
  }


}//end namespace nexus
