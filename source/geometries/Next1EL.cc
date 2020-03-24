// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors:  <justo.martin-albo@ific.uv.es>
//                <paola.ferrario@ific.uv.es>
//  Created: 27 Jan 2010
//
//  Copyright (c) 2010, 2011 NEXT Collaboration
// ----------------------------------------------------------------------------


#include "Next1EL.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "PmtR7378A.h"
#include "IonizationSD.h"
#include "PmtSD.h"
#include "HexagonPointSampler.h"
#include "UniformElectricDriftField.h"
#include "XenonGasProperties.h"
#include "NextElDB.h"
#include "CylinderPointSampler.h"
#include "MuonsPointSampler.h"
#include "Visibilities.h"

#include <G4Orb.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4Polyhedra.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4Region.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UserLimits.hh>
#include <G4RotationMatrix.hh>
#include <G4Transform3D.hh>
#include <G4RunManager.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4GenericMessenger.hh>
#include <G4UnitsTable.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

using namespace nexus;
using namespace CLHEP;

Next1EL::Next1EL():
  BaseGeometry(),
  // LABORATORY //////////////////////////////////
  _lab_size (2. * m),
  // VESSEL //////////////////////////////////////
  _vessel_diam   (300. * mm),
  _vessel_length (600. * mm),
  _vessel_thickn (  3. * mm),
  // ENDCAPS /////////////////////////////////////
  _endcap_diam   (330. * mm),
  _endcap_thickn ( 30. * mm),
  // SIDE SOURCE-PORT ////////////////////////////
  _sideport_diam   (40. * mm),
  _sideport_length (30. * mm),
  _sideport_thickn ( 2. * mm),
  _sideport_flange_diam   (71. * mm),
  _sideport_flange_thickn ( 8. * mm),
  _sideport_tube_diam   (12.1 * mm),
  _sideport_tube_length (30.0 * mm),
  _sideport_tube_thickn ( 1.0 * mm),
  _sideport_tube_window_thickn (0.5 * mm),
  // AXIAL SOURCE-PORT ///////////////////////////
  _axialport_diam   (16. * mm),
  _axialport_length (24. * mm),
  _axialport_thickn ( 2. * mm),
  _axialport_flange_diam   (34. * mm),
  _axialport_flange_thickn (13. * mm),
  _axialport_tube_diam   ( 5. * mm),
  _axialport_tube_length (30. * mm),
  _axialport_tube_thickn ( 1. * mm),
  _axialport_tube_window_thickn (0.5 * mm),
  // ELECTROLUMINESCENCE GAP /////////////////////
  _elgap_length (5. * mm),
  _elgap_ring_diam   (229. * mm),
  _elgap_ring_thickn ( 12. * mm),
  _elgap_ring_height (  5. * mm),
  // _wire_diam(.003048 * cm),
  // TPB
  _tpb_thickn (.001 * mm),
  // LIGHT TUBE //////////////////////////////////
  _ltube_diam      (160. * mm),
  _ltube_thickn    (  5. * mm + _tpb_thickn),
  _ltube_up_length (295. * mm),
  _ltube_bt_length (100. * mm),
  _ltube_gap       (  3. * mm),
  // ACTIVE VOLUME ///////////////////////////////
  _active_diam   (_ltube_diam),
  _active_length (300. * mm),
  // FIELD CAGE //////////////////////////////////
  _fieldcage_length (_elgap_ring_height + _elgap_length +
		     _active_length + _ltube_gap +
		     _ltube_bt_length),
  _fieldcage_displ  (84.*mm),
  // FIELD SHAPING RINGS /////////////////////////
  _ring_diam   (229. * mm),
  // _ring_height ( 10. * mm),
  _ring_thickn (  6. * mm),
  // SUPPORT BARS ////////////////////////////////
  _bar_width  ( 40. * mm),
  _bar_thickn ( 10. * mm),
  _bar_addon_length (50. * mm),
  // PMT ENERGY PLANE ////////////////////////////
  _pmtholder_cath_diam         (229. * mm),
  _pmtholder_cath_height       ( 40. * mm),
  _pmtholder_cath_cutout_depth ( 10. * mm),
  _pmtholder_cath_displ (1. * cm),
  _pmt_pitch (34.59 * mm),
  // PMT TRACKING PLANE //////////////////////////
  _pmtholder_anode_diam   (229. * mm),
  _pmtholder_anode_thickn ( 13. * mm),
  // DEFAULT VALUES FOR SOME PARAMETERS///////////
  _max_step_size(1.*mm),
  _sc_yield(13889/MeV),
  _attachment(1000.*ms)
{

  _msg = new G4GenericMessenger(this, "/Geometry/Next1EL/",
				"Control commands of geometry Next1EL.");

  // Boolean-type properties (true or false)
  _msg->DeclareProperty("elfield", _elfield,
			"True if the EL field is on (full simulation), false if it's not (parametrized simulation.");
  _msg->DeclareProperty("tpb_coating", _tpb_coating,
			"True if the upper light tube is coated.");
  _msg->DeclareProperty("external_scintillator", _external_scintillator,
			"True if the an external NaI scintillator is used.");


  // String-like properties
  _msg->DeclareProperty("tracking_plane", _tracking_plane,
			"PMT or SIPM, according to what tracking plane it's being used.");

  new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

  G4GenericMessenger::Command& sc_yield_cmd =
    _msg->DeclareProperty("sc_yield", _sc_yield,
			  "Set scintillation yield for GXe. It is in photons/MeV");
  sc_yield_cmd.SetParameterName("sc_yield", true);
  sc_yield_cmd.SetUnitCategory("1/Energy");

  G4GenericMessenger::Command& attachment_cmd =
    _msg->DeclareProperty("attachment", _attachment,
        "Electron attachment in gas. It is in milliseconds");
  attachment_cmd.SetParameterName("attachment", false);
  attachment_cmd.SetRange("attachment>0.");

  /// Temporary
  G4GenericMessenger::Command&  specific_vertex_X_cmd =
    _msg->DeclareProperty("specific_vertex_X", _specific_vertex_X,
   			  "If region is AD_HOC, x coord where particles are generated");
  specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
  specific_vertex_X_cmd.SetUnitCategory("Length");
  G4GenericMessenger::Command&  specific_vertex_Y_cmd =
    _msg->DeclareProperty("specific_vertex_Y", _specific_vertex_Y,
   			  "If region is AD_HOC, y coord where particles are generated");
  specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
  specific_vertex_Y_cmd.SetUnitCategory("Length");
  G4GenericMessenger::Command&  specific_vertex_Z_cmd =
    _msg->DeclareProperty("specific_vertex_Z", _specific_vertex_Z,
   			  "If region is AD_HOC, z coord where particles are generated");
  specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
  specific_vertex_Z_cmd.SetUnitCategory("Length");

   // Other properties (dimension and dimensionless)
  G4GenericMessenger::Command& pressure_cmd =
    _msg->DeclareProperty("pressure", _pressure,
			  "Set pressure for gaseous xenon.");
  pressure_cmd.SetUnitCategory("Pressure");
  pressure_cmd.SetParameterName("pressure", false);
  pressure_cmd.SetRange("pressure>0.");

  G4GenericMessenger::Command& sideport_angle_cmd =
    _msg->DeclareProperty("sideport_angle", _sideport_angle,
  			  "Set angle of sideport.");
  sideport_angle_cmd.SetUnitCategory("Angle");

  G4GenericMessenger::Command& sideport_posz_cmd =
    _msg->DeclareProperty("sideport_posz", _sideport_posz,
			  "Set distance of sideport from z=0.");
  sideport_posz_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command& max_step_size_cmd =
    _msg->DeclareProperty("max_step_size", _max_step_size,
			  "Set maximum step size.");
  max_step_size_cmd.SetUnitCategory("Length");
  max_step_size_cmd.SetParameterName("max_step_size", true);
  max_step_size_cmd.SetRange("max_step_size>0");

  G4GenericMessenger::Command& elgrid_transparency_cmd =
    _msg->DeclareProperty("elgrid_transparency", _elgrid_transparency,
			  "Set the transparency of the anode EL mesh.");
  elgrid_transparency_cmd.SetParameterName("elgrid_transparency", false);
  elgrid_transparency_cmd.SetRange("elgrid_transparency>0 && elgrid_transparency<1");

  G4GenericMessenger::Command& gate_transparency_cmd =
    _msg->DeclareProperty("gate_transparency", _gate_transparency,
			  "Set the transparency of the gate EL mesh.");
  gate_transparency_cmd.SetParameterName("gate_transparency", false);
  gate_transparency_cmd.SetRange("gate_transparency>0 && gate_transparency<1");


  //muons building
  G4GenericMessenger::Command& muonsGenerator_cmd =_msg->DeclareProperty("muonsGenerator", _muonsGenerator,
			"Build or not Muons");
  muonsGenerator_cmd.SetParameterName("muonsGenerator", false);

}



Next1EL::~Next1EL()
{
  delete _hexrnd;
  delete _muons_sampling;
  delete _msg;
  delete _cps;
}


void Next1EL::Construct()
{
  DefineMaterials();
  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones
  BuildLab();
  if(_muonsGenerator)
    BuildMuons();
  if (_external_scintillator)
    BuildExtScintillator();
  BuildVessel();
  BuildFieldCage();
  BuildEnergyPlane();

  if (_tracking_plane == "SIPM")
    BuildSiPMTrackingPlane();
  else
    BuildPMTTrackingPlane();

  G4ThreeVector v(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
  _specific_vertex = v;

   // For EL Table generation
  _idx_table = 0;
  _table_vertices.clear();
  /// Ionielectrons are generated at a z = .5 mm inside the EL gap
  G4double z = _vessel_length/2. - _fieldcage_displ - _elgap_ring_height - _elgap_length;
  SetELzCoord(z);
  z = z +  .5*mm;
  CalculateELTableVertices(92.5*mm, 5.*mm, z);

}




void Next1EL::DefineMaterials()
{
  // AIR
  _air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  // GASEOUS XENON
  _gxe = MaterialsList::GXe(_pressure, 303);
  _gxe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, 303, _sc_yield, _attachment));

  G4cout << "Scintillation yield: " << _sc_yield << G4endl;


  // PTFE (TEFLON)
  _teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  // STAINLESS STEEL
  _steel = MaterialsList::Steel();
  // ALUMINUM
  _aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
  //LEAD
  _lead = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
  //PLASTIC
  _plastic = MaterialsList::PS();
  //TPB
  _tpb = MaterialsList::TPB();
  _tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
}



void Next1EL::BuildLab()
{
  // LAB /////////////////////////////////////////////////////////////
  // This is just a volume of air surrounding the detector so that
  // events (from calibration sources or cosmic rays) can be generated
  // on the outside.

  G4Box* lab_solid =
    new G4Box("LAB", _lab_size/2., _lab_size/2., _lab_size/2.);

  _lab_logic = new G4LogicalVolume(lab_solid, _air, "LAB");
  _lab_logic->SetVisAttributes(G4VisAttributes::Invisible);
  this->SetDrift(true);

  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(_lab_logic);
}

void Next1EL::BuildMuons()
{
  // MUONS /////////////////////////////////////////////////////////////

  G4double xMuons =  _lab_size/3;
  G4double yMuons = _lab_size/500.;
  G4double zMuons = _lab_size/2;

  G4double yMuonsOrigin = 400.;

  //sampling position in a surface above the detector
  _muons_sampling = new MuonsPointSampler(xMuons, yMuonsOrigin, zMuons);


  // To visualize the muon generation surface

  //visualization sphere
  // G4Orb * muon_solid_ref = new G4Orb ("MUONS_ref",25);
  // G4LogicalVolume*  muon_logic_ref = new G4LogicalVolume(muon_solid_ref, _air, "MUONS_ref");
  // new G4PVPlacement(0, G4ThreeVector(0., yMuonsOrigin, 0.), muon_logic_ref,
  //                             "MUONS_ref", _lab_logic, false, 0, true);

  G4Box* muon_solid =
    new G4Box("MUONS", xMuons, yMuons, zMuons);
  G4LogicalVolume*  muon_logic = new G4LogicalVolume(muon_solid, _air, "MUONS");
  new G4PVPlacement(0, G4ThreeVector(0., yMuonsOrigin, 0.), muon_logic,
		    "MUONS", _lab_logic, false, 0, true);

   // visualization
  G4VisAttributes muon_col = nexus::Red();
  muon_col.SetForceSolid(true);
  muon_logic->SetVisAttributes(muon_col);
}


void Next1EL::BuildExtScintillator()
{
  G4double vessel_total_diam = _vessel_diam + 2.*_vessel_thickn;
  // rotation of the source-port
  G4RotationMatrix rot;
  rot.rotateY(-pi/2.);
  rot.rotateZ(_sideport_angle);

  _sideNa_pos = G4ThreeVector((-vessel_total_diam/2. - _sideport_length -
			_sideport_flange_thickn)
		       * cos(_sideport_angle),
			(-vessel_total_diam/2. - _sideport_length -
			 _sideport_flange_thickn)
		       * sin(_sideport_angle),
		       _sideport_posz);

///The source itself
  G4double source_thick = .1*mm;
  G4double source_diam = 3.*mm;
  G4Tubs* source_solid =
    new G4Tubs("SOURCE", 0., source_diam/2., source_thick/2., 0., twopi);
  G4Material* sodium22_mat =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_Na");
  G4LogicalVolume* source_logic =
    new G4LogicalVolume(source_solid, sodium22_mat, "SOURCE");

  G4ThreeVector pos_source =
    G4ThreeVector( _sideNa_pos.getX()-source_thick/2.*cos(_sideport_angle),
		   _sideNa_pos.getY()-source_thick/2.*sin(_sideport_angle),
		   _sideNa_pos.getZ());

  new G4PVPlacement(G4Transform3D(rot, pos_source), source_logic, "SOURCE",
		    _lab_logic, false, 0, true);
  G4VisAttributes source_col = nexus::LightGreen();
  source_col.SetForceSolid(true);
  source_logic->SetVisAttributes(source_col);

   G4RotationMatrix* rot2 = new  G4RotationMatrix();
   rot2->rotateY(-pi/2.);
   rot2->rotateZ(_sideport_angle);

   _cps =
     new CylinderPointSampler(source_diam/2., source_thick, 0., 0., pos_source, rot2);


  ///Plastic support
  G4double support_thick = 6.2*mm;
  G4double support_diam = 25.33*mm;
  G4Tubs* support_solid =
    new G4Tubs("SUPPORT", 0., support_diam/2., support_thick/2., 0., twopi);
  G4LogicalVolume* support_logic =
    new G4LogicalVolume(support_solid, _plastic, "SUPPORT");
  G4ThreeVector pos_support =
    G4ThreeVector(pos_source.getX()-(support_thick+source_thick)/2.*cos(_sideport_angle),
		  pos_source.getY()-(support_thick+source_thick)/2.*sin(_sideport_angle),
		  pos_source.getZ());
  new G4PVPlacement(G4Transform3D(rot, pos_support), support_logic,
		    "SOURCE_SUPPORT",  _lab_logic, false, 0, true);

  G4VisAttributes support_col = nexus::Red();
  support_col.SetForceSolid(true);
  support_logic->SetVisAttributes(support_col);


  /// NaI scintillator behind
  G4double dist_sc = 5.*cm;
  G4double radius = 44.5/2.*mm;
  G4double length = 38.7*mm;
  G4Tubs* sc_solid = new G4Tubs("NaI", 0., radius, length/2., 0., twopi);
  G4Material* mat =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_SODIUM_IODIDE");
  G4LogicalVolume* sc_logic = new G4LogicalVolume(sc_solid, mat, "NaI");
  sc_logic->SetUserLimits(new G4UserLimits(1.*mm));

  // G4ThreeVector pos_scint =
  //   G4ThreeVector(pos_al.getX()-(al_thick/2.+dist_sc)*cos(_sideport_angle),
  // 		  pos_al.getY()-(al_thick/2.+dist_sc)*sin(_sideport_angle),
  // 		  sideNa.getZ());
  G4ThreeVector pos_scint =
    G4ThreeVector(pos_support.getX() -
		  (support_thick/2.+dist_sc+length/2.)*cos(_sideport_angle),
		  pos_support .getY()-(support_thick/2.+dist_sc+length/2.)*sin(_sideport_angle),
		  _sideNa_pos.getZ());
  new G4PVPlacement(G4Transform3D(rot, pos_scint), sc_logic, "NaI",
		    _lab_logic, false, 0, true);
  G4VisAttributes naI_col = nexus::Blue();
  naI_col.SetForceSolid(true);
  sc_logic->SetVisAttributes(naI_col);


  // NaI is defined as an ionization sensitive volume.
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
  G4String detname = "/NEXT1/SCINT";
  IonizationSD* ionisd = new IonizationSD(detname);
  ionisd->IncludeInTotalEnergyDeposit(false);
  sdmgr->AddNewDetector(ionisd);
  sc_logic->SetSensitiveDetector(ionisd);


}



void Next1EL::BuildVessel()
{
  // CAN /////////////////////////////////////////////////////////////
  // Stainless steel (grade 304L) cylinder. CF-300.
  // A CF-60 half-nipple on the side accommodates a window for
  // calibration sources.

  G4double vessel_total_diam = _vessel_diam + 2.*_vessel_thickn;

  // To avoid overlaps between volumes, the can solid volume
  // is the result of the union of the port and the CF-300 cylinder.

  G4Tubs* can_solid = new G4Tubs("VESSEL", 0., vessel_total_diam/2.,
				 _vessel_length/2., 0, twopi);

  G4Tubs* sideport_solid =
    new G4Tubs("VESSEL", 0., (_sideport_diam/2. + _sideport_thickn),
	       _sideport_length, 0, twopi);

  // rotation matrix for the side source port
  G4RotationMatrix rotport;
  rotport.rotateY(-pi/2.);
  rotport.rotateZ(_sideport_angle);

  // position of the side source-port
  G4ThreeVector posport(-vessel_total_diam/2. * cos(_sideport_angle),
			-vessel_total_diam/2. * sin(_sideport_angle),
			_sideport_posz);

  G4UnionSolid* vessel_solid =
    new G4UnionSolid("VESSEL", can_solid, sideport_solid,
		     G4Transform3D(rotport, posport));

  G4LogicalVolume* vessel_logic =
    new G4LogicalVolume(vessel_solid, _steel, "VESSEL");

  new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_logic,
		    "VESSEL", _lab_logic, false, 0, true);


  // GAS /////////////////////////////////////////////////////////////
  // Gas filling the can.
  // Obviously, it has the same shape of the can, thus requiring
  // another union between solid volumes.

  G4Tubs* cyl_gas =
    new G4Tubs("GAS", 0., _vessel_diam/2., _vessel_length/2., 0, twopi);

  G4Tubs* sideport_gas =
    new G4Tubs("GAS", 0., _sideport_diam/2.,
	       (_sideport_length + _vessel_thickn), 0, twopi);

  posport.setX(-_vessel_diam/2. * cos(_sideport_angle));
  posport.setY(-_vessel_diam/2. * sin(_sideport_angle));

  G4UnionSolid* gas_solid = new G4UnionSolid("GAS", cyl_gas, sideport_gas,
					     G4Transform3D(rotport, posport));

  _gas_logic = new G4LogicalVolume(gas_solid, _gxe, "GAS");
  _gas_logic->SetVisAttributes(G4VisAttributes::Invisible);

  new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), _gas_logic,
		    "GAS", vessel_logic, false, 0, true);


  // SIDE SOURCE-PORT ////////////////////////////////////////////////

  // FLANGE ................................................
  // CF flange closing the port.

  G4Tubs* sideport_flange_solid =
    new G4Tubs("SIDEPORT_FLANGE", _sideport_tube_diam/2.,
	       _sideport_flange_diam/2., _sideport_flange_thickn/2., 0, twopi);

  G4LogicalVolume* sideport_flange_logic =
    new G4LogicalVolume(sideport_flange_solid, _steel, "SIDEPORT_FLANGE");

  G4double radial_pos =
    -(vessel_total_diam/2. + _sideport_length + _sideport_flange_thickn/2.);

  posport.setX(radial_pos * cos(_sideport_angle));
  posport.setY(radial_pos * sin(_sideport_angle));

  new G4PVPlacement(G4Transform3D(rotport, posport), sideport_flange_logic,
		    "SIDEPORT_FLANGE", _lab_logic, false, 0, true);
  G4VisAttributes * vis = new G4VisAttributes;
  vis->SetColor(0.5, 0.5, .5);
  vis->SetForceSolid(true);
  sideport_flange_logic->SetVisAttributes(vis);

  radial_pos = -(vessel_total_diam/2. + _sideport_length + _sideport_flange_thickn);
  _sideport_ext_position.setX(radial_pos * cos(_sideport_angle));
  _sideport_ext_position.setY(radial_pos * sin(_sideport_angle));
  _sideport_ext_position.setZ(_sideport_posz);



  // TUBE ..................................................

  G4Tubs* sideport_tube_solid =
    new G4Tubs("SIDEPORT", 0., (_sideport_tube_diam/2.+_sideport_tube_thickn),
   	       _sideport_tube_length/2., 0, twopi);

  G4LogicalVolume* sideport_tube_logic =
    new G4LogicalVolume(sideport_tube_solid, _steel, "SIDEPORT");

  radial_pos =
    -(vessel_total_diam/2. + _sideport_length - _sideport_tube_length/2.);
  posport.setX(radial_pos * cos(_sideport_angle));
  posport.setY(radial_pos * sin(_sideport_angle));

  _sideport_position = posport;

  new G4PVPlacement(G4Transform3D(rotport, posport), sideport_tube_logic,
		    "SIDEPORT",  _gas_logic, false, 0, true);

  G4Tubs* sideport_tube_air_solid =
    new G4Tubs("SIDEPORT_AIR", 0., _sideport_tube_diam/2.,
	       (_sideport_tube_length - _sideport_tube_window_thickn)/2.,
	       0, twopi);

  G4LogicalVolume* sideport_tube_air_logic =
    new G4LogicalVolume(sideport_tube_air_solid, _air, "SIDEPORT_AIR");

  new G4PVPlacement(0,G4ThreeVector(0.,0.,_sideport_tube_window_thickn/2.),
		    sideport_tube_air_logic, "SIDEPORT_AIR",
		    sideport_tube_logic, false, 0, true);


  // CATHODE ENDCAP //////////////////////////////////////////////////
  // Flat endcap, CF-300, no ports of interest for the simulation.

  G4Tubs* endcap_cathode_solid =
    new G4Tubs("ENDCAP_CATHODE", 0., _endcap_diam/2.,
	       _endcap_thickn/2., 0., twopi);

  G4LogicalVolume* endcap_cathode_logic =
    new G4LogicalVolume(endcap_cathode_solid, _steel, "ENDCAP_CATHODE");

  G4double posz = (_vessel_length + _endcap_thickn) / 2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,-posz), endcap_cathode_logic,
  		    "ENDCAP_CATHODE", _lab_logic, false, 0, true);


  // ANODE ENDCAP ////////////////////////////////////////////////////
  // Flat endcap, CF-300, CF-16 port for calibration sources
  // Unlike the side-port, union between the endcap and port logical
  // volumes is not required. The endcap will have a central hole
  // where the port will be positioned.

  G4Tubs* endcap_anode_solid =
    new G4Tubs("ENDCAP_ANODE", (_axialport_diam/2. + _axialport_thickn),
	       _endcap_diam/2., _endcap_thickn/2., 0., twopi);

  G4LogicalVolume* endcap_anode_logic =
    new G4LogicalVolume(endcap_anode_solid, _steel, "ENDCAP_ANODE");

  new G4PVPlacement(0, G4ThreeVector(0., 0., posz), endcap_anode_logic,
		    "ENDCAP_ANODE", _lab_logic, false, 0, true);


  // AXIAL SOURCE-PORT ///////////////////////////////////////////////

  // PORT ..................................................

  G4double axialport_total_length = _endcap_thickn + _axialport_length;

  G4Tubs* axialport_solid =
    new G4Tubs("AXIALPORT", 0., (_axialport_diam/2. + _axialport_thickn),
	       axialport_total_length/2., 0, twopi);

  G4LogicalVolume* axialport_logic =
    new G4LogicalVolume(axialport_solid, _steel, "AXIALPORT");

  posz = (_vessel_length + axialport_total_length) / 2.;

  _axialport_position.setX(0.);
  _axialport_position.setY(0.);
  _axialport_position.setZ(posz);

  new G4PVPlacement(0, _axialport_position, axialport_logic,
   		    "AXIALPORT", _lab_logic, false, 0, true);

  G4Tubs* axialport_gas_solid = new G4Tubs("GAS", 0., _axialport_diam/2.,
					   axialport_total_length/2., 0, twopi);

  G4LogicalVolume* axialport_gas_logic =
    new G4LogicalVolume(axialport_gas_solid, _gxe, "GAS");

  new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), axialport_gas_logic,
   		    "GAS", axialport_logic, false, 0, true);


  // FLANGE ................................................

  G4Tubs* axialport_flange_solid =
    new G4Tubs("AXIALPORT_FLANGE", _axialport_tube_diam/2.,
   	       _axialport_flange_diam/2., _axialport_flange_thickn/2., 0, twopi);

  G4LogicalVolume* axialport_flange_logic =
    new G4LogicalVolume(axialport_flange_solid, _steel, "AXIALPORT_FLANGE");

  posz = _vessel_length/2. + _endcap_thickn +
    _axialport_length + _axialport_flange_thickn/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), axialport_flange_logic,
   		    "AXIALPORT_FLANGE", _lab_logic, false, 0, true);

  // Store the position of the port so that it can be used in vertex generation
  posz = posz + _axialport_flange_thickn/2.;
  _axialport_position.set(0., 0., posz);


  // SOURCE TUBE ...........................................

  G4Tubs* axialport_tube_solid =
    new G4Tubs("AXIALPORT", 0., (_axialport_tube_diam/2.+_axialport_tube_thickn),
   	       _axialport_tube_length/2., 0, twopi);

  G4LogicalVolume* axialport_tube_logic =
    new G4LogicalVolume(axialport_tube_solid, _steel, "AXIALPORT");

  posz = (axialport_total_length - _axialport_tube_length) / 2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), axialport_tube_logic,
		    "AXIALPORT", axialport_gas_logic, false, 0, true);

  G4Tubs* axialport_tube_air_solid =
    new G4Tubs("AXIALPORT_AIR", 0., _axialport_tube_diam/2.,
   	       (_axialport_tube_length-_axialport_tube_window_thickn)/2.,
	       0, twopi);

  G4LogicalVolume* axialport_tube_air_logic =
    new G4LogicalVolume(axialport_tube_air_solid, _air, "AXIALPORT_AIR");

  new G4PVPlacement(0, G4ThreeVector(0,0,_axialport_tube_window_thickn/2.),
   		    axialport_tube_air_logic, "AXIALPORT_AIR",
		    axialport_tube_logic, false, 0, true);
}



void Next1EL::BuildFieldCage()
{
  // Store the position of the center of the fieldcage
  _fieldcage_position.
    set(0., 0., (_vessel_length/2.-_fieldcage_length/2.-_fieldcage_displ));

  // Position of the electrodes in the fieldcage
  _anode_posz   = _fieldcage_length/2. - _elgap_ring_height;
  _gate_posz    = _anode_posz - _elgap_length;
  _cathode_posz = _gate_posz - _active_length;


  // ELECTROLUMINESCENCE GAP /////////////////////////////////////////

  // GATE AND ANODE (EL) RINGS .............................

  G4Tubs* elgap_ring_solid =
    new G4Tubs("EL_GAP_RING", _elgap_ring_diam/2.,
	       (_elgap_ring_diam/2. + _elgap_ring_thickn),
	       _elgap_ring_height/2., 0, twopi);

  G4LogicalVolume* elgap_ring_logic =
    new G4LogicalVolume(elgap_ring_solid, _aluminum, "EL_GAP_RING");

  G4double posz = _fieldcage_length/2. - _elgap_ring_height/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), elgap_ring_logic,
   		    "EL_GAP_RING", _gas_logic, false, 0, true);

  posz = posz - _elgap_ring_height - _elgap_length;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), elgap_ring_logic,
   		    "EL_GAP_RING", _gas_logic, false, 1, true);


  // EL GAP ................................................

  G4Tubs* elgap_solid = new G4Tubs("EL_GAP", 0., _elgap_ring_diam/2.,
   				   _elgap_length/2., 0, twopi);

  G4LogicalVolume* elgap_logic =
    new G4LogicalVolume(elgap_solid, _gxe, "EL_GAP");

  posz = _fieldcage_length/2. - _elgap_ring_height - _elgap_length/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), elgap_logic,
		    "EL_GAP", _gas_logic, false, 0, true);


  // Store the position of the EL GAP wrt the WORLD system of reference
  _elgap_position.set(0.,0.,_fieldcage_position.z()+posz);

  // EL GRIDS

  G4double diel_thickn = .1*mm;

  G4Material* fgrid = MaterialsList::FakeDielectric(_gxe, "grid_mat");
  fgrid->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, 303,
									_elgrid_transparency, diel_thickn, _sc_yield, _attachment));
  G4Material* fgrid_gate = MaterialsList::FakeDielectric(_gxe, "grid_mat");
  fgrid_gate->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, 303,
									_gate_transparency, diel_thickn, _sc_yield, _attachment));

  G4Tubs* diel_grid =
    new G4Tubs("GRID", 0., _elgap_ring_diam/2., diel_thickn/2., 0, twopi);


  G4LogicalVolume* diel_grid_gate_logic =
    new G4LogicalVolume(diel_grid, fgrid_gate, "GRID_GATE");
  G4double pos1 = - _elgap_length/2. + diel_thickn/2.;
  new G4PVPlacement(0, G4ThreeVector(0.,0.,pos1), diel_grid_gate_logic, "GRID_GATE",
		    elgap_logic, false, 0, true);

  G4LogicalVolume* diel_grid_logic =
    new G4LogicalVolume(diel_grid, fgrid, "GRID");
  G4double pos2 = _elgap_length/2. - diel_thickn/2.;
  new G4PVPlacement(0, G4ThreeVector(0.,0.,pos2), diel_grid_logic, "GRID",
		    elgap_logic, false, 1, true);

  if (_elfield) {
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    el_field->SetCathodePosition(_elgap_position.z()-_elgap_length/2.);
    el_field->SetAnodePosition(_elgap_position.z()+_elgap_length/2.);
    el_field->SetDriftVelocity(2.5*mm/microsecond);
    el_field->SetTransverseDiffusion(1.*mm/sqrt(cm));
    el_field->SetLongitudinalDiffusion(.5*mm/sqrt(cm));
    XenonGasProperties xgp(_pressure, 303);
    // el_field->SetLightYield(xgp.ELLightYield(24.8571*kilovolt/cm));//value for E that gives Y=1160 photons per ie- in normal conditions
    el_field->SetLightYield(xgp.ELLightYield(23.2857*kilovolt/cm));
    G4Region* el_region = new G4Region("EL_REGION");
    el_region->SetUserInformation(el_field);
    el_region->AddRootLogicalVolume(elgap_logic);
  }

  // ACTIVE VOLUME ///////////////////////////////////////////////////

  // Position of z planes
  G4double zplane[2] = {-_active_length/2., _active_length/2.};
  // Inner radius
  G4double rinner[2] = {0., 0.};
  // Outer radius
  G4double router[2] = {_active_diam/2., _active_diam/2.};

  G4Polyhedra* active_solid =
    new G4Polyhedra("ACTIVE", 0., twopi, 6, 2, zplane, rinner, router);

  G4LogicalVolume* active_logic =
    new G4LogicalVolume(active_solid, _gxe, "ACTIVE");

  posz = _fieldcage_length/2. - _elgap_ring_height -
    _elgap_length - _active_length/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), active_logic,
		    "ACTIVE", _gas_logic, false, 0, true);

  // Store the position of the active volume with respect to the
  // WORLD system of reference
  _active_position.set(0.,0.,_fieldcage_position.z()+posz);

  _hexrnd = new HexagonPointSampler(_active_diam/2., _active_length, 0.,
				    _active_position);

  // Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(_max_step_size));

  // Set the volume as an ionization sensitive detector
  G4String det_name = "/NEXT1/ACTIVE";
  IonizationSD* ionisd = new IonizationSD(det_name);
  active_logic->SetSensitiveDetector(ionisd);
  G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

  //Define a drift field for this volume
  UniformElectricDriftField* field = new UniformElectricDriftField();
  field->SetCathodePosition(_active_position.z() - _active_length/2.);
  field->SetAnodePosition(_elgap_position.z()-_elgap_length/2.);
  field->SetDriftVelocity(1.*mm/microsecond);
  field->SetTransverseDiffusion(1.*mm/sqrt(cm));
  field->SetLongitudinalDiffusion(.3*mm/sqrt(cm));

  G4Region* drift_region = new G4Region("DRIFT");
  drift_region->SetUserInformation(field);
  drift_region->AddRootLogicalVolume(active_logic);

  active_logic->SetVisAttributes(G4VisAttributes::Invisible);


  // LIGHT TUBE //////////////////////////////////////////////////////
  // Hexagonal prism of teflon used as light tube. It transports the
  // EL light from anode to cathode improving the light collection
  // efficiency of the energy plane.

  // UPPER PART ............................................

  zplane[0] = -_ltube_up_length/2.; zplane[1] = _ltube_up_length/2.;
  rinner[0] = _ltube_diam/2.; rinner[1] = rinner[0];
  router[0] = _ltube_diam/2. + _ltube_thickn; router[1] = router[0];

  G4Polyhedra* ltube_up_solid =
    new G4Polyhedra("LIGHT_TUBE_UP", 0., twopi, 6, 2, zplane, rinner, router);

  G4LogicalVolume* ltube_up_logic =
    new G4LogicalVolume(ltube_up_solid, _teflon, "LIGHT_TUBE_UP");

  posz = (_gate_posz + _cathode_posz) / 2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), ltube_up_logic,
   		    "LIGHT_TUBE_UP", _gas_logic, false, 0, true);

  // TPB coating
  if (_tpb_coating) {
    G4double rinner_tpb[2];
    G4double router_tpb[2];
    rinner_tpb[0] = _ltube_diam/2.; rinner_tpb[1] = rinner_tpb[0];
    router_tpb[0] = _ltube_diam/2. + _tpb_thickn; router_tpb[1] = router_tpb[0];

    G4Polyhedra* ltube_tpb_solid =
      new G4Polyhedra("LIGHT_TUBE_TPB", 0., twopi, 6, 2, zplane,
		      rinner_tpb, router_tpb);
    G4LogicalVolume* ltube_tpb_logic =
      new G4LogicalVolume(ltube_tpb_solid, _tpb, "LIGHT_TUBE_TPB");

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), ltube_tpb_logic,
		      "LIGHT_TUBE_TPB", ltube_up_logic, false, 0, true);

    G4VisAttributes * visattrib_red = new G4VisAttributes;
    visattrib_red->SetColor(1., 0., 0.);
    ltube_tpb_logic->SetVisAttributes(visattrib_red);
  }

  // BOTTOM PART ...........................................

  zplane[0] = -_ltube_bt_length/2.; zplane[1] = _ltube_bt_length/2.;

  G4Polyhedra* ltube_bt_solid =
    new G4Polyhedra("LIGHT_TUBE_BOTTOM", 0., twopi, 6, 2, zplane, rinner, router);

  G4LogicalVolume* ltube_bt_logic =
    new G4LogicalVolume(ltube_bt_solid, _teflon, "LIGHT_TUBE_BOTTOM");

  posz = _fieldcage_length/2. - _elgap_ring_height - _elgap_length -
    _active_length - _ltube_gap - _ltube_bt_length/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), ltube_bt_logic,
   		    "LIGHT_TUBE_BOTTOM", _gas_logic, false, 0, true);


  // FIELD SHAPING RINGS /////////////////////////////////////////////

  // G4Tubs* ring_solid = new G4Tubs("FIELD_RING", _ring_diam/2.,
  //  				  (_ring_diam/2.+_ring_thickn),
  // 				  _ring_height/2., 0, twopi);

  // G4LogicalVolume* ring_logic =
  //   new G4LogicalVolume(ring_solid, _aluminum, "FIELD_RING");


  // // DRIFT REGION ................................

  // G4int num_rings = 19;

  // posz = _fieldcage_length/2. - 2.*_elgap_ring_height
  //   - _elgap_length - 5.*mm - _ring_height/2.;

  // for (G4int i=0; i<num_rings; i++) {

  //   new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
  //     		      "FIELD_RING", fieldcage_logic, false, i, true);

  //   posz = posz - _ring_height - 5.1 * mm;
  // }


  // // BUFFER ......................................

  // posz = _fieldcage_length/2. - _ring_height - _elgap_length
  //   - _ltube_gap - _ltube_up_length - _ltube_gap - 10.*mm - _ring_height/2.;

  // for (G4int i=19; i<23; i++) {

  //    new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
  //     		      "FIELD_RING", fieldcage_logic, false, i, true);

  //   posz = posz - _ring_height - 10. * mm;
  // }


  // CATHODE .....................................

  diel_thickn = 1. * mm;
  G4double transparency = 0.98;

  G4Material* fcathode = MaterialsList::FakeDielectric(_gxe, "cathode_mat");
  fcathode->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, 303,
  									transparency, diel_thickn, _sc_yield, _attachment));

  G4Tubs* diel_cathd =
    new G4Tubs("CATHODE", 0., _elgap_ring_diam/2., diel_thickn/2., 0, twopi);

  G4LogicalVolume* diel_cathd_logic =
    new G4LogicalVolume(diel_cathd, fcathode, "CATHODE");

  posz = _cathode_posz - diel_thickn/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), diel_cathd_logic, "CATHODE",
		    _gas_logic, false, 0, true);


  // SUPPORT BARS ////////////////////////////////////////////////////

  G4double bar_length = _fieldcage_length - 2.*_elgap_ring_height - _elgap_length;

  G4Box* bar_base = new G4Box("SUPPORT_BAR", _bar_thickn/2.,
			 _bar_width/2., bar_length/2.);

  G4Box* addon = new G4Box("SUPPORT_BAR", _bar_thickn/2.,
			   _bar_width/2., _bar_addon_length/2.);

  G4UnionSolid* bar_solid =
    new G4UnionSolid("SUPPORT_BAR", bar_base, addon, 0,
		     G4ThreeVector(_bar_thickn, 0., (bar_length - _bar_addon_length)/2.));

  G4LogicalVolume* bar_logic =
    new G4LogicalVolume(bar_solid, MaterialsList::PEEK(), "SUPPORT_BAR");


  G4double pos_rad = _ring_diam/2. + _ring_thickn + _bar_thickn/2.;
  posz = _fieldcage_length/2. - 2.*_elgap_ring_height -
    _elgap_length - bar_length/2.;

  for (G4int i=0; i<6; i++) {

    G4RotationMatrix rotbar;
    rotbar.rotateZ(i*60.*deg);

    G4double xx = pos_rad * cos(i*60.*deg);
    G4double yy = pos_rad * sin(i*60.*deg);
    G4double zz = posz;

    new G4PVPlacement(G4Transform3D(rotbar, G4ThreeVector(xx, yy, zz + _fieldcage_position.z())),
		      bar_logic, "SUPPORT_BAR", _gas_logic, false, i, true);
  }

  // OPTICAL SURFACES ////////////////////////////////////////////////
  G4OpticalSurface* ltubeup_opsur = new G4OpticalSurface("LIGHT_TUBE_UP");
  ltubeup_opsur->SetType(dielectric_metal);
  ltubeup_opsur->SetModel(unified);
  ltubeup_opsur->SetFinish(ground);
  ltubeup_opsur->SetSigmaAlpha(0.1);
  ltubeup_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  G4OpticalSurface* ltubebt_opsur = new G4OpticalSurface("LIGHT_TUBE_BOTTOM");
  ltubebt_opsur->SetType(dielectric_metal);
  ltubebt_opsur->SetModel(unified);
  ltubebt_opsur->SetFinish(ground);
  ltubebt_opsur->SetSigmaAlpha(0.1);
  ltubebt_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface("LIGHT_TUBE_UP",     ltube_up_logic, ltubeup_opsur);
  new G4LogicalSkinSurface("LIGHT_TUBE_BOTTOM", ltube_bt_logic, ltubebt_opsur);
}



void Next1EL::BuildEnergyPlane()
{
  // PMT HOLDER //////////////////////////////////////////////////////
  // Teflon plate that holds the PMTs. Its front face has a hexagonal
  // cutout matching the size and shape of the light tube.

  G4Tubs* cyl_holder =
    new G4Tubs("PMT_HOLDER_CATHODE", 0., _pmtholder_cath_diam/2.,
  	       _pmtholder_cath_height/2., 0, twopi);

  G4double zplane[2] = {-_pmtholder_cath_cutout_depth/2.,
			_pmtholder_cath_cutout_depth/2.};
  G4double router[2] = {_ltube_diam/2., _ltube_diam/2.};
  G4double rinner[2] = {0., 0.};

  G4Polyhedra* cutout_holder =
    new G4Polyhedra("PMT_HOLDER_CATHODE", 0, twopi, 6, 2,
		    zplane, rinner, router);

  // The holder solid volume results from the subtraction of the two
  // previous volumes. In order to avoid the generation of a 'fake'
  // surface due to numerical precision loss, the two solids should
  // not have any common surface. Therefore, we add a slight security
  // margin to the translation of the cutout

  G4double secmargin = 0.5 * mm;
  G4double transl_z =
    _pmtholder_cath_height/2. + secmargin - _pmtholder_cath_cutout_depth/2.;

  G4SubtractionSolid* pmtholder_solid =
    new G4SubtractionSolid("PMT_HOLDER", cyl_holder, cutout_holder,
  			   0, G4ThreeVector(0, 0, transl_z));

  // The holder has holes were the PMTs are fitted.
  // (The placement is done below, in the same loop where we'll place
  // the PMTs.)

  const G4double hole_diam = 26. * mm;
  const G4double hole_depth = _pmtholder_cath_height;

  G4Tubs* hole_holder = new G4Tubs("PMT_HOLDER_CATHODE", 0., hole_diam/2.,
				   hole_depth/2., 0., twopi);

  // Z position of the PMT holder
  transl_z = _fieldcage_position.z() -
    (_fieldcage_length/2. + _pmtholder_cath_displ + _pmtholder_cath_height/2.);

  // PHOTOMULTIPLIERS ////////////////////////////////////////////////

  // Load the geometry model of the PMT and get the pointer
  // to its logical volume
  PmtR7378A pmt;
  pmt.Construct();
  _pmt_logic = pmt.GetLogicalVolume();

  // The PMTs are placed in the holder in a honeycomb arrangement.
  // We use the hexagon point sampler to calculate the positions of the
  // PMTs given the pitch.
  HexagonPointSampler hexsampler(_ltube_diam/2., 0., 0., G4ThreeVector(0.,0.,0.));
  hexsampler.TesselateWithFixedPitch(_pmt_pitch, _pmt_positions);

  // Loop over the vector of positions
  for (unsigned int i = 0; i<_pmt_positions.size(); i++) {

    // Make a hole for the PMT in the holder
    pmtholder_solid =
      new G4SubtractionSolid("PMT_HOLDER_CATHODE", pmtholder_solid,
			     hole_holder, 0, _pmt_positions[i]);

    // Place the PMT.
    // Notice that the PMT is not positioned inside the holder but
    // inside the gas. Therefore, the position (to be specific, the Z
    // coordinate) must be given in coordinates relative to the world.
    new G4PVPlacement(0, G4ThreeVector(_pmt_positions[i].x(),
  				       _pmt_positions[i].y(),
  				       transl_z-0.5*cm),
  		      _pmt_logic, "PMT", _gas_logic, false, i, true);
  }

  // Finish with the positioning of the PMT holder

  G4LogicalVolume* pmtholder_logic =
    new G4LogicalVolume(pmtholder_solid, _teflon, "PMT_HOLDER_CATHODE");

  new G4PVPlacement(0, G4ThreeVector(0.,0.,transl_z), pmtholder_logic,
  		    "PMT_HOLDER_CATHODE", _gas_logic, false, 0);



  // OPTICAL SURFACE ////////////////////////////////////////////////

  G4OpticalSurface* pmtholder_cath_opsur = new G4OpticalSurface("PMT_HOLDER_CATHODE");
  pmtholder_cath_opsur->SetType(dielectric_metal);
  pmtholder_cath_opsur->SetModel(unified);
  pmtholder_cath_opsur->SetFinish(ground);
  pmtholder_cath_opsur->SetSigmaAlpha(0.1);
  pmtholder_cath_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface("PMT_HOLDER_CATHODE", pmtholder_logic, pmtholder_cath_opsur);


  // PMT SHIELD //////////////////////////////////////////////////////

  G4double shield_thickn = 1. * mm;
  G4double transparency = 0.96;

  G4Material* fshield = MaterialsList::FakeDielectric(_gxe, "shield_mat");
  fshield->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, 303,
  									transparency, shield_thickn, _sc_yield, _attachment));

  G4Tubs* shield_solid =
    new G4Tubs("PMT_SHIELD", 0., _elgap_ring_diam/2., shield_thickn, 0, twopi);

  G4LogicalVolume* shield_logic =
    new G4LogicalVolume(shield_solid, fshield, "PMT_SHIELD");

  transl_z = _fieldcage_position.z() -
    (_fieldcage_length/2. + _pmtholder_cath_displ/2.);

  new G4PVPlacement(0, G4ThreeVector(0.,0.,transl_z), shield_logic,
  		    "PMT_SHIELD", _gas_logic, false, 0);

}


void Next1EL::BuildSiPMTrackingPlane()
{

   /* New Dice Board configuration */
  NextElDB db(8,8);
  db.Construct();

  G4LogicalVolume* db_logic = db.GetLogicalVolume();
  G4double db_xsize = db.GetDimensions().x();
  G4double db_ysize = db.GetDimensions().y();
  G4double db_zsize = db.GetDimensions().z();

  G4RotationMatrix* rotdb = new G4RotationMatrix();
  rotdb->rotateZ(pi);

  /// Distance of SiPM surface from the beginning of EL region
  G4double dist_el = 7.5*mm;
  G4double z = _elgap_position.z() + _elgap_length/2. + dist_el
    + db_zsize/2.;

   G4double gap = 1.*mm;
   G4int db_no = 1;

   for (G4int j=0; j<2; ++j) {
     G4double y = gap/2. + db_ysize/2. - j*(gap + db_xsize);
     for (G4int i=0; i<2; ++i) {
       G4double x =  - gap/2.- db_xsize/2. + i*(gap + db_xsize);
       if (j == 0) {
  	 new G4PVPlacement(rotdb, G4ThreeVector(x,y,z), db_logic,
  			   "DB", _gas_logic, false, db_no);
       } else {
  	 new G4PVPlacement(0, G4ThreeVector(x,y,z), db_logic,
  			   "DB", _gas_logic, false, db_no);
       }
       std::vector<std::pair<int, G4ThreeVector> > positions = db.GetPositions();
       for (unsigned int si=0; si<positions.size(); si++) {
  	 G4ThreeVector mypos = positions[si].second;
  	 G4ThreeVector mypos_rot = (*rotdb)*mypos;
  	 std::pair<int, G4ThreeVector> abs_pos;
  	 abs_pos.first = db_no*1000+positions[si].first ;
  	 if (j == 0) {
  	   abs_pos.second = G4ThreeVector(x+mypos_rot.getX(), y+mypos_rot.getY(), z+mypos.getZ());
  	 } else {
  	   abs_pos.second = G4ThreeVector(x+mypos.getX(),y+mypos.getY(),z+mypos.getZ());
  	 }
  	 _absSiPMpos.push_back(abs_pos);
       }
       db_no++;
     }
   }

  //  PrintAbsoluteSiPMPos();

   /* First Daughter Board configuration */
  // Next1ELDBO dbo44_geom(4,4);
  // Next1ELDBO dbo43_geom(4,3);
  // Next1ELDBO dbo42_geom(4,2);

  // G4LogicalVolume* dbo44_logic = dbo44_geom.GetLogicalVolume();
  // G4LogicalVolume* dbo43_logic = dbo43_geom.GetLogicalVolume();
  // G4LogicalVolume* dbo42_logic = dbo42_geom.GetLogicalVolume();

  // G4RotationMatrix* rotdbo = new G4RotationMatrix();
  // rotdbo->rotateY(pi);

  // G4double dbo44_xsize = dbo44_geom.GetDimensions().x();
  // G4double dbo44_ysize = dbo44_geom.GetDimensions().y();
  // G4double dbo43_xsize = dbo43_geom.GetDimensions().x();
  // G4double dbo43_ysize = dbo43_geom.GetDimensions().y();
  // G4double dbo42_xsize = dbo42_geom.GetDimensions().x();
  // G4double dbo42_ysize = dbo42_geom.GetDimensions().y();
  // // G4cout << "Size of dbo: " << dbo44_xsize << ", " << dbo44_ysize << G4endl;
  // G4double gap = 2.*mm;

  // G4int dbo_no = 0;

  // for (G4int i=0; i<4; i++) {
  //   G4double y = 3/2.*gap/2. + 3./2.*dbo44_ysize - i * (dbo44_ysize+gap);
  //   for (G4int j=0; j<3; j++) {
  //     G4double x = - dbo44_xsize - gap + j * (dbo44_xsize+gap);
  //     new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo44_logic,
  //  			"DBO44", _gas_logic, false, dbo_no, true);
  //     dbo_no++;
  //   }
  // }

  // G4double x = - 3./2.*dbo44_xsize - 2.*gap - dbo42_xsize/2.;
  // G4double y = gap + dbo44_ysize;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo42_logic,
  //  			"DBO42", _gas_logic, false, 13, true);
  // x = -x;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo42_logic,
  // 		    "DBO42", _gas_logic, false, 14, true);
  // y = -y;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo42_logic,
  // 		    "DBO42", _gas_logic, false, 15, true);
  // x = -x;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo42_logic,
  // 		    "DBO42", _gas_logic, false, 16, true);

  // x = - 3./2.*dbo44_xsize - 2.*gap - dbo43_xsize/2.;
  // y = 0;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo43_logic,
  // 		    "DBO43", _gas_logic, false, 17, true);
  // x = -x;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,240.*mm), dbo43_logic,
  // 		    "DBO43", _gas_logic, false, 18, true);



}

void Next1EL::BuildPMTTrackingPlane()
{
  // PMT HOLDER /////////////////////////////////////////////////////

  G4Tubs* cyl_holder =
    new G4Tubs("PMT_HOLDER_ANODE", 0., _pmtholder_anode_diam/2.,
	       _pmtholder_anode_thickn/2., 0., twopi);

  const G4double hole_diam  = 26. * mm;

  G4Tubs* hole_holder = new G4Tubs("PMT_HOLDER_ANODE", 0., hole_diam/2.,
				   _pmtholder_anode_thickn, 0., twopi);

  // Make the first hole to create the volume (the solid volume
  // has to exist already to use it in a subtraction, as we'll do
  // in the loop below).
  G4SubtractionSolid* pmtholder_solid =
    new G4SubtractionSolid("PMT_HOLDER_ANODE", cyl_holder, hole_holder,
			   0, _pmt_positions[0]);


  // PHOTOMULTIPLIERS ///////////////////////////////////////////////

  //Distance of PMT surface from the beginning of EL region
  G4double dist_el = 2.5*mm;
  // PMT position in gas
  PmtR7378A pmt;
  G4double pmt_length = pmt.Length();
  G4double posz = _elgap_position.z() + _elgap_length/2. + dist_el + pmt_length/2.;

  G4RotationMatrix rotpmt;
  rotpmt.rotateX(pi);


  // Since we had to make the central hole already to create the solid
  // volume, we place the central PMT outside the loop
  new G4PVPlacement(G4Transform3D(rotpmt,
				  G4ThreeVector(_pmt_positions[0].x(),
						_pmt_positions[0].y(),
						posz)),
		    _pmt_logic, "PMT", _gas_logic, false, 19);

  // the anode plane is rotated 0 degrees around the z axis
  G4RotationMatrix rotanode;
  rotanode.rotateZ(0.);

  std::vector<G4ThreeVector> pmt_pos_rot;
  pmt_pos_rot.push_back(G4ThreeVector(0., 0., 0.));

  for (unsigned int i=1; i<_pmt_positions.size(); i++){

    G4ThreeVector pos(_pmt_positions[i].x(),
			 _pmt_positions[i].y(),
			 0.);
    pos = rotanode*pos;
    pmt_pos_rot.push_back(pos);
  }

  // Loop over the PMT positions
  for (unsigned int i=1; i<pmt_pos_rot.size(); i++) {

    G4int pmt_no = i + _pmt_positions.size();

    pmtholder_solid =
      new G4SubtractionSolid("PMT_HOLDER_ANODE", pmtholder_solid,
			     hole_holder, 0, pmt_pos_rot[i]);

    new G4PVPlacement(G4Transform3D(rotpmt,
    				    G4ThreeVector(pmt_pos_rot[i].x(),
    						  pmt_pos_rot[i].y(),
    						  posz)),
    		      _pmt_logic, "PMT", _gas_logic, false, pmt_no);
  }

  G4LogicalVolume* pmtholder_logic =
    new G4LogicalVolume(pmtholder_solid, _teflon, "PMT_HOLDER_ANODE");

  new G4PVPlacement(0, G4ThreeVector(0,0,posz), pmtholder_logic,
   		    "PMT_HOLDER_ANODE", _gas_logic, false, 0);

  // OPTICAL SURFACE ////////////////////////////////////////////////

  G4OpticalSurface* pmtholder_anode_opsur = new G4OpticalSurface("PMT_HOLDER_ANODE");
  pmtholder_anode_opsur->SetType(dielectric_metal);
  pmtholder_anode_opsur->SetModel(unified);
  pmtholder_anode_opsur->SetFinish(ground);
  pmtholder_anode_opsur->SetSigmaAlpha(0.1);
  pmtholder_anode_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface("PMT_HOLDER_ANODE", pmtholder_logic, pmtholder_anode_opsur);
}

G4ThreeVector Next1EL::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex(0., 0., 0.);
  if (region == "CENTER") {
    vertex = _active_position;
  } else if (region == "SIDEPORT") {
    vertex = _sideport_ext_position;
  } else if (region == "AXIALPORT") {
    vertex = _axialport_position;
  } else if (region == "Na22LATERAL") {
    //    G4ThreeVector point = _sideNa_pos;
    vertex = _sideNa_pos;
  } else if (region == "ACTIVE") {
    vertex =  _hexrnd->GenerateVertex(INSIDE);
  } else if (region == "RESTRICTED") {
    vertex =  _hexrnd->GenerateVertex(PLANE);
    //  } else if (region == "FIXED_RADIUS") {
    //  G4ThreeVector point = _hexrnd->GenerateVertex(RADIUS, 10.);
    // G4cout <<  point.getX() << ", "
    // 	   <<  point.getY() << ", "
    // 	   <<  point.getZ() << G4endl;
    //    return  point;
  } else if (region == "AD_HOC"){
    vertex =  _specific_vertex;
  } else if (region == "EL_TABLE") {
      _idx_table++;
      if(_idx_table>=_table_vertices.size()){
    	G4cout<<"[Next1EL] Aborting the run, last event reached ..."<<G4endl;
    	G4RunManager::GetRunManager()->AbortRun();
      }
      if(_idx_table<=_table_vertices.size()){
    	vertex =  _table_vertices[_idx_table-1];
      }
  } else if (region == "MUONS") {
    //generate muons sampling the plane
    vertex = _muons_sampling->GenerateVertex();

  } else {
      G4Exception("[Next1EL]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

  return vertex;


}

void Next1EL::PrintAbsoluteSiPMPos()
{
  G4cout << "----- Absolute position of SiPMs in gas volume -----" << G4endl;
  G4cout <<  G4endl;
  for (unsigned int i=0; i<_absSiPMpos.size(); i++) {
    std::pair<int, G4ThreeVector> abs_pos = _absSiPMpos[i];
    G4cout << "ID number: " << _absSiPMpos[i].first << ", position: "
    	   << _absSiPMpos[i].second.getX() << ", "
    	   << _absSiPMpos[i].second.getY() << ", "
    	   << _absSiPMpos[i].second.getZ()  << G4endl;
  }
  G4cout <<  G4endl;
  G4cout << "---------------------------------------------------" << G4endl;
}

void Next1EL::CalculateELTableVertices(G4double radius,
				       G4double binning, G4double z)
{
    G4ThreeVector position;

    ///Squared pitch
    position[2] = z;

    G4int imax = floor(2*radius/binning);

    // const G4double a = 92.38;
    // const G4double b = 46.19;
    // const G4double c = 80.;

    for (int i=0; i<imax; i++){
      position[0] = -radius + i*binning;
      for (int j=0; j<imax; j++){
	position[1] = -radius + j*binning;
	//	G4cout << position[0] << ", " << position[1] << G4endl;
	if (sqrt(position[0]*position[0]+position[1]*position[1])< radius){
	  _table_vertices.push_back(position);

	} else {
	  continue;
	}
      }
    }
}
