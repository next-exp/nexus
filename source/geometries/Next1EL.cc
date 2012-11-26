// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>, <paola.ferrario@ific.uv.es>
//  Created: 27 Jan 2010
//  
//  Copyright (c) 2010, 2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next1EL.h"

#include "ConfigService.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "PmtR7378A.h"
#include "IonizationSD.h"
#include "PmtSD.h"
#include "HexagonPointSampler.h"
#include "UniformElectricDriftField.h"
#include "XenonGasProperties.h"
#include "Next1ELDBO.h"
#include "NextElDB.h"
#include "BhepUtils.h"

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

using namespace nexus;



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
// TPB /////////////////////////////////////////
  _tpb_thickn(.001 * mm),
// LIGHT TUBE //////////////////////////////////
  _ltube_diam      (160. * mm),
  _ltube_thickn    (  5. * mm + _tpb_thickn),
  _ltube_up_length (297.5 * mm),
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
  _ring_height ( 10. * mm),
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
  ////////////////////////////////////////////////
  _binning(0.), _idx_table(0)
{
  ReadParameters();
  DefineMaterials();

  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones
  BuildLab();
  BuildVessel();
  BuildFieldCage();
  BuildEnergyPlane();
  if (_tracking_plane == "SIPM") 
    BuildSiPMTrackingPlane();
  else 
    BuildPMTTrackingPlane();
}



Next1EL::~Next1EL()
{
  _table_vertices.clear();
  delete _hexrnd;
}
  
  
  
void Next1EL::ReadParameters()
{
  const ParamStore& cfg_geom = ConfigService::Instance().Geometry();

  // Xenon gas pressure
  _pressure = cfg_geom.GetDParam("pressure");

  // Tracking plane type
  _tracking_plane = cfg_geom.GetSParam("tracking_plane");

  // Position of the side source-port
  _sideport_angle = cfg_geom.GetDParam("sideport_angle") * degree;
  _sideport_posz  = cfg_geom.GetDParam("sideport_posz");

  // Maximum step size for electrons in active volume (default: 1 mm)
  if (cfg_geom.PeekDParam("max_step_size"))
    _max_step_size = cfg_geom.GetDParam("max_step_size");
  else
    _max_step_size = 1. * mm;

  // Placement of TPB coating
  _tpb_coating = cfg_geom.GetIParam("tpb_coating");

  // Transparency of EL meshes
  if (cfg_geom.PeekDParam("elgrid_transparency")) {
    _elgrid_transparency = cfg_geom.GetDParam("elgrid_transparency");
  } else {
    _elgrid_transparency = .80;
  }
  
  
  const ParamStore& cfg_gen = ConfigService::Instance().Generation();

  // Generation vertex specified by user
  if (cfg_gen.GetSParam("region") == "AD_HOC") {
    _specific_vertex = 
      BhepUtils::DVto3Vector(cfg_gen.GetDVParam("specific_vertex"));
  }
  // Needed for lookup table generation
  if (cfg_gen.GetSParam("generator_name") == "EL_LOOKUP_TABLE")
    _binning = cfg_gen.GetDParam("binning");
  
      
  const ParamStore& cfg_job = ConfigService::Instance().Job();
  _numb_of_events = cfg_job.GetIParam("number_events");
}
  
  
  
void Next1EL::DefineMaterials()
{
  // AIR
  _air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  // GASEOUS XENON
  _gxe = MaterialsList::GXe(_pressure, 303);
  _gxe->
    SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, 303));
  // PTFE (TEFLON)
  _teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  // STAINLESS STEEL
  _steel = MaterialsList::Steel();
  // ALUMINUM
  _aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
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

  // Set this volume as the wrapper for the whole geometry 
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(_lab_logic);
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
   	       _axialport_flange_diam/2., _axialport_flange_thickn/2., 
	       0, twopi);

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
    new G4Tubs("AXIALPORT", 
	       0., (_axialport_tube_diam/2.+_axialport_tube_thickn),
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
  // FIELD CAGE //////////////////////////////////////////////////////
  // This is just a virtual wrapper volume for all the parts and pieces
  // that constitute the detector field cage. It encloses the following
  // volumes: active, the two EL gap rings, EL gap, 
  // upper and bottom light tubes.
  // 
  // The _fieldcage_length variable is its dimension along the z axis, 
  // and it is the sum of the following lengths:
  // _elgap_ring_height + _elgap_length + _active_length + _ltube_gap 
  // + _ltube_bt_length.
  // The _fieldcage_displ variable is the distance between the end of 
  // the VESSEL volume and the beginning of the most external 
  // EL_GAP_RING volume
 
  // Position of the electrodes in the WORLD
  _anode_posz   = _vessel_length/2. - _fieldcage_displ - _elgap_ring_height;
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

  G4double posz = _vessel_length/2.-_fieldcage_displ - _elgap_ring_height/2.;
  
  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), elgap_ring_logic,
   		    "EL_GAP_RING", _gas_logic, false, 0, true);
  
  posz = posz - _elgap_ring_height - _elgap_length;
  
  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), elgap_ring_logic,
   		    "EL_GAP_RING", _gas_logic, false, 1, true);
  G4VisAttributes vis_solid;
  vis_solid.SetForceSolid(true);
  elgap_ring_logic->SetVisAttributes(vis_solid);
  
  
  // EL GAP ................................................
  
  G4Tubs* elgap_solid = new G4Tubs("EL_GAP", 0., _elgap_ring_diam/2.,
   				   _elgap_length/2., 0, twopi);
  
  G4LogicalVolume* elgap_logic = 
    new G4LogicalVolume(elgap_solid, _gxe, "EL_GAP");
  
  posz = _vessel_length/2. - _fieldcage_displ - _elgap_ring_height 
    - _elgap_length/2.;
    
  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), elgap_logic,
		    "EL_GAP", _gas_logic, false, 0, true);


  // Store the position of the EL GAP wrt the WORLD system of reference
  _elgap_position.set(0.,0.,posz);

  if (_binning > 0.) 
    CalculateELTableVertices(_binning);

  
  // EL GRIDS
  
  G4double diel_thickn = .1*mm;

  G4Material* fgrid = MaterialsList::CopyMaterial(_gxe, "grid_mat");
  fgrid->SetMaterialPropertiesTable
    (OpticalMaterialProperties::FakeGrid(_pressure, 303, _elgrid_transparency,
					 diel_thickn));
  
  G4double pos1 = - _elgap_length/2. + diel_thickn/2.;
  G4double pos2 = _elgap_length/2. - diel_thickn/2.;

  G4Tubs* diel_grid =  
    new G4Tubs("GRID", 0., _elgap_ring_diam/2., diel_thickn/2., 0, twopi);

  G4LogicalVolume* diel_grid_logic =
    new G4LogicalVolume(diel_grid, fgrid, "GRID");

  G4PVPlacement* diel_grid_physi;

  diel_grid_physi =
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pos1), diel_grid_logic, "GRID",
    		      elgap_logic, false, 0, true);
  diel_grid_physi =
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pos2), diel_grid_logic, "GRID",
		      elgap_logic, false, 1, true);

  //UniformElectricDriftField* el_field = new UniformElectricDriftField();
  // el_field->SetCathodePosition(_elgap_position.z()-_elgap_length/2.);
  // el_field->SetAnodePosition(_elgap_position.z()+_elgap_length/2.);
  // el_field->SetDriftVelocity(5.*mm/microsecond);
  // el_field->SetTransverseDiffusion(1.*mm/sqrt(cm));
  // el_field->SetLongitudinalDiffusion(.5*mm/sqrt(cm));
  // //Set light yield for EL region
  // XenonGasProperties xgp =  XenonGasProperties(_pressure, 303);
  // const G4double yield = xgp.ELLightYield(30*kilovolt/cm);
  // el_field->SetLightYield(yield);

  // G4Region* el_region = new G4Region("EL_REGION");
  // el_region->SetUserInformation(el_field);
  // el_region->AddRootLogicalVolume(elgap_logic);
  
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
 
  posz = _vessel_length/2. -_fieldcage_displ - _elgap_ring_height - 
    _elgap_length - _active_length/2.;
    
  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), active_logic, 
		    "ACTIVE", _gas_logic, false, 0, true);

  // Store the position of the active volume with respect to the
  // WORLD system of reference
  _active_position.set(0.,0.,posz);

  _hexrnd = new HexagonPointSampler(_active_diam/2., _active_length, 0.,
				    _active_position);

  // Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(_max_step_size));
    
  // Set the volume as an ionization sensitive detector
  // IonizationSD* ionisd = new IonizationSD("/NEXT1/ACTIVE");
  // active_logic->SetSensitiveDetector(ionisd);
  // G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

  //Define a drift field for this volume
  UniformElectricDriftField* field = new UniformElectricDriftField();
  field->SetCathodePosition(_active_position.z() - _active_length/2.);
  field->SetAnodePosition(_elgap_position.z()-_elgap_length/2.);
  field->SetDriftVelocity(1.*mm/microsecond);
  field->SetTransverseDiffusion(1.*mm/sqrt(cm));
  field->SetLongitudinalDiffusion(.5*mm/sqrt(cm));
  
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
    new G4Polyhedra("LIGHT_TUBE", 0., twopi, 6, 2, zplane, rinner, router);
  
  G4LogicalVolume* ltube_up_logic = 
    new G4LogicalVolume(ltube_up_solid, _teflon, "LIGHT_TUBE");

  posz =  _vessel_length/2. -_fieldcage_displ - _elgap_ring_height 
    - _elgap_length - _elgap_ring_height/2.  - _ltube_up_length/2.;
    

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), ltube_up_logic, 
   		    "LIGHT_TUBE", _gas_logic, false, 0, true);
  
  G4VisAttributes * visattrib_blue = new G4VisAttributes;
  visattrib_blue->SetColor(0., 0., 1.);
  ltube_up_logic->SetVisAttributes(visattrib_blue);
 
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
    new G4Polyhedra("LIGHT_TUBE", 0., twopi, 6, 2, zplane, rinner, router);
    
  G4LogicalVolume* ltube_bt_logic = 
    new G4LogicalVolume(ltube_bt_solid, _teflon, "LIGHT_TUBE");

  posz = _vessel_length/2. - _fieldcage_displ - _elgap_ring_height 
    - _elgap_length - _active_length - _ltube_gap - _ltube_bt_length/2.;
  
  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), ltube_bt_logic, 
   		    "LIGHT_TUBE", _gas_logic, false, 0, true);
 
  ltube_bt_logic->SetVisAttributes(visattrib_blue);
  
  // FIELD SHAPING RINGS /////////////////////////////////////////////
    
  G4Tubs* ring_solid = new G4Tubs("FIELD_RING", _ring_diam/2.,
   				  (_ring_diam/2.+_ring_thickn),
				  _ring_height/2., 0, twopi);
    
  G4LogicalVolume* ring_logic =
    new G4LogicalVolume(ring_solid, _aluminum, "FIELD_RING");

  
  // DRIFT REGION ................................

  G4int num_rings = 19;

  posz = _vessel_length/2.-_fieldcage_displ - 2.*_elgap_ring_height 
    - _elgap_length - 5.*mm - _ring_height/2.;

  for (G4int i=0; i<num_rings; i++) {
      
    new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
  		      "FIELD_RING", _gas_logic, false, i, true);
      
    posz = posz - _ring_height - 5.1 * mm;
  }

  
  // BUFFER ......................................

  posz = _vessel_length/2.-_fieldcage_displ  - _ring_height - _elgap_length
    - _ltube_gap - _ltube_up_length - _ltube_gap - 10.*mm - _ring_height/2.;

  for (G4int i=19; i<23; i++) {

    new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
  		      "FIELD_RING", _gas_logic, false, i, true);
      
    posz = posz - _ring_height - 10. * mm;
  }

  ring_logic->SetVisAttributes(vis_solid);
  
  // CATHODE .....................................

  diel_thickn = 1. * mm;
  G4double transparency = 0.98;

  G4Material* fcathode = MaterialsList::CopyMaterial(_gxe, "cathode_mat");
  fcathode->SetMaterialPropertiesTable
    (OpticalMaterialProperties::FakeGrid(_pressure, 303, transparency, 
					 diel_thickn));

  G4Tubs* diel_cathd =  
    new G4Tubs("CATHODE", 0., _elgap_ring_diam/2., diel_thickn/2., 0, twopi);

  G4LogicalVolume* diel_cathd_logic =
    new G4LogicalVolume(diel_cathd, fcathode, "CATHODE");

  posz = _vessel_length/2. - _fieldcage_displ - _elgap_ring_height 
    - _elgap_length - _active_length - diel_thickn/2.;
 
  G4PVPlacement* diel_cathd_physi =
    new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), diel_cathd_logic, 
		      "CATHODE", _gas_logic, false, 0, true);

  G4VisAttributes * visattrib_green = new G4VisAttributes;
  visattrib_green->SetColor(0., 1., 0.);
  diel_cathd_logic->SetVisAttributes(visattrib_green);
   
    
  // SUPPORT BARS ////////////////////////////////////////////////////
 
  G4double bar_length = _fieldcage_length - 2.*_elgap_ring_height 
    - _elgap_length;

  G4Box* bar = new G4Box("SUPPORT_BAR", _bar_thickn/2., 
			 _bar_width/2., bar_length/2.);
  
  G4Box* addon = new G4Box("SUPPORT_BAR", _bar_thickn/2., 
			   _bar_width/2., _bar_addon_length/2.);

  G4UnionSolid* bar_solid = 
    new G4UnionSolid("SUPPORT_BAR", bar, addon, 0, 
		     G4ThreeVector(_bar_thickn, 0., 
				   (bar_length - _bar_addon_length)/2.));

  G4LogicalVolume* bar_logic =
    new G4LogicalVolume(bar_solid, MaterialsList::PEEK(), "SUPPORT_BAR");


  G4double pos_rad = _ring_diam/2. + _ring_thickn + _bar_thickn/2.;
 
  posz = _vessel_length/2. - _fieldcage_displ  - 2.*_elgap_ring_height - 
    _elgap_length - bar_length/2.;
  
  for (G4int i=0; i<6; i++) {
    
    G4RotationMatrix rotbar;
    rotbar.rotateZ(i*60.*deg);
    
    G4double xx = pos_rad * cos(i*60.*deg);
    G4double yy = pos_rad * sin(i*60.*deg);
    G4double zz = posz;
    
    new G4PVPlacement(G4Transform3D(rotbar, G4ThreeVector(xx, yy, zz)), 
     		      bar_logic, "SUPPORT_BAR", _gas_logic, false, i, true);
  }
  
    
  // OPTICAL SURFACES ////////////////////////////////////////////////

  G4OpticalSurface* ltubeup_opsur = new G4OpticalSurface("LIGHT_TUBE_UP");
  ltubeup_opsur->SetType(dielectric_metal);
  ltubeup_opsur->SetModel(unified);
  ltubeup_opsur->SetFinish(ground);
  ltubeup_opsur->SetSigmaAlpha(0.1);
  if (_tpb_coating) {
    ltubeup_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_with_TPB());
  } else {
    ltubeup_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
  }

  G4OpticalSurface* ltubebt_opsur = new G4OpticalSurface("LIGHT_TUBE_BOTTOM");
  ltubebt_opsur->SetType(dielectric_metal);
  ltubebt_opsur->SetModel(unified);
  ltubebt_opsur->SetFinish(ground);
  ltubebt_opsur->SetSigmaAlpha(0.1);
  ltubebt_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface("LIGHT_TUBE", ltube_up_logic, ltubeup_opsur);
  new G4LogicalSkinSurface("LIGHT_TUBE", ltube_bt_logic, ltubebt_opsur);

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
  transl_z =  _vessel_length/2.-_fieldcage_displ -_fieldcage_length - 
    _pmtholder_cath_displ - _pmtholder_cath_height/2.;
  
  
  // PHOTOMULTIPLIERS ////////////////////////////////////////////////

  // Load the geometry model of the PMT and get the pointer
  // to its logical volume
  PmtR7378A pmt;
  _pmt_logic = pmt.GetLogicalVolume();
  
  // The PMTs are placed in the holder in a honeycomb arrangement. 
  // We use the hexagon point sampler to calculate the positions of the
  // PMTs given the pitch.
  HexagonPointSampler hexsampler(_ltube_diam/2., 0., 0., 
				 G4ThreeVector(0.,0.,0.));
  hexsampler.TesselateWithFixedPitch(_pmt_pitch, _pmt_positions);

  // Loop over the vector of positions
  for (G4int i = 0; i<_pmt_positions.size(); i++) {
    
    // Make a hole for the PMT in the holder
    pmtholder_solid = 
      new G4SubtractionSolid("PMT_HOLDER_CATHODE", pmtholder_solid, 
			     hole_holder, 0, _pmt_positions[i]);
    
    // Place the PMT. Notice that it is not positioned inside the holder 
    // but inside the gas. Therefore, the position (to be specific, the 
    // Z coordinate) must be given in coordinates relative to the world.
    new G4PVPlacement(0, G4ThreeVector(_pmt_positions[i].x(), 
  				       _pmt_positions[i].y(),
  				       transl_z - 5.*mm), 
  		      _pmt_logic, "PMT", _gas_logic, false, i, true);
  }
  
  // Finish with the positioning of the PMT holder
  
  G4LogicalVolume* pmtholder_logic =
    new G4LogicalVolume(pmtholder_solid, _teflon, "PMT_HOLDER_CATHODE");
  
  new G4PVPlacement(0, G4ThreeVector(0.,0.,transl_z), pmtholder_logic,
  		    "PMT_HOLDER_CATHODE", _gas_logic, false, 0, true);

  // OPTICAL SURFACE ////////////////////////////////////////////////

  G4OpticalSurface* pmtholder_cath_opsur = 
    new G4OpticalSurface("PMT_HOLDER_CATHODE");
  pmtholder_cath_opsur->SetType(dielectric_metal);
  pmtholder_cath_opsur->SetModel(unified);
  pmtholder_cath_opsur->SetFinish(ground);
  pmtholder_cath_opsur->SetSigmaAlpha(0.1);
  pmtholder_cath_opsur->
    SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface("PMT_HOLDER_CATHODE", 
			   pmtholder_logic, pmtholder_cath_opsur);
  
  // PMT SHIELD //////////////////////////////////////////////////////
  // Grid of parallel wires, 0.004" diameter, separated 5 mm, resulting
  // in a ~98% optical transparency.
  
  const G4double shield_thickn = 1. * mm;
  const G4double transparency = 0.96;
 
  G4Material* fshield = MaterialsList::CopyMaterial(_gxe, "shield_mat");
  fshield->
    SetMaterialPropertiesTable(OpticalMaterialProperties::
			       FakeGrid(_pressure, 303, transparency, 
					shield_thickn));
  
  G4Tubs* shield_solid = 
    new G4Tubs("PMT_SHIELD", 0., _elgap_ring_diam/2., shield_thickn, 0, twopi);

  G4LogicalVolume* shield_logic =
    new G4LogicalVolume(shield_solid, fshield, "PMT_SHIELD");

  transl_z = _vessel_length/2.-_fieldcage_displ - _fieldcage_length 
    - _pmtholder_cath_displ/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,transl_z), shield_logic, 
  		    "PMT_SHIELD", _gas_logic, false, 0, true);
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
  G4double pos_z = _elgap_position.z() + _elgap_length/2. + dist_el 
    + pmt_length/2.; 

  G4RotationMatrix rotpmt;
  rotpmt.rotateX(pi);


  // Since we had to make the central hole already to create the solid 
  // volume, we place the central PMT outside the loop
  new G4PVPlacement(G4Transform3D(rotpmt, 
				  G4ThreeVector(_pmt_positions[0].x(), 
						_pmt_positions[0].y(),
						pos_z)), 
		    _pmt_logic, "PMT", _gas_logic, false, 19, true);


  // Loop over the PMT positions
  for (G4int i=1; i<_pmt_positions.size(); i++) {

    G4int pmt_no = i + _pmt_positions.size();

    pmtholder_solid = 
      new G4SubtractionSolid("PMT_HOLDER_ANODE", pmtholder_solid, 
			     hole_holder, 0, _pmt_positions[i]);

    new G4PVPlacement(G4Transform3D(rotpmt, 
    				    G4ThreeVector(_pmt_positions[i].x(), 
    						  _pmt_positions[i].y(),
    						  pos_z)), 
    		      _pmt_logic, "PMT", _gas_logic, false, pmt_no, true);
  }
  
  G4LogicalVolume* pmtholder_logic =
    new G4LogicalVolume(pmtholder_solid, _teflon, "PMT_HOLDER_ANODE");
  
  new G4PVPlacement(0, G4ThreeVector(0,0,pos_z), pmtholder_logic,
   		    "PMT_HOLDER_ANODE", _gas_logic, false, 0, true);

  // OPTICAL SURFACE ////////////////////////////////////////////////

  G4OpticalSurface* pmtholder_anode_opsur = 
    new G4OpticalSurface("PMT_HOLDER_ANODE");
  pmtholder_anode_opsur->SetType(dielectric_metal);
  pmtholder_anode_opsur->SetModel(unified);
  pmtholder_anode_opsur->SetFinish(ground);
  pmtholder_anode_opsur->SetSigmaAlpha(0.1);
  pmtholder_anode_opsur->
    SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface("PMT_HOLDER_ANODE", 
			   pmtholder_logic, pmtholder_anode_opsur);
  G4VisAttributes vis_solid;
  vis_solid.SetForceSolid(true);
  pmtholder_logic->SetVisAttributes(vis_solid);
}



void Next1EL::BuildSiPMTrackingPlane()
{
  /* New Dice Board configuration */
  NextElDB db(8,8);
  G4LogicalVolume* db_logic = db.GetLogicalVolume();
  G4double db_xsize = db.GetDimensions().x();
  G4double db_ysize = db.GetDimensions().y();
  G4double db_zsize = db.GetDimensions().z();

  /// Distance of SiPM surface from the beginning of EL region
  G4double dist_el = 2.5*mm;
  G4double z = _elgap_position.z() + _elgap_length/2. + dist_el 
    + db_zsize/2.; 

   G4double gap = 1.*mm;
   G4int db_no = 1;

   for (G4int j=0; j<2; ++j) {
     G4double y = gap/2. + db_ysize/2. - j*(gap + db_xsize);
     for (G4int i=0; i<2; ++i) {
       G4double x =  - gap/2.- db_xsize/2. + i*(gap + db_xsize);
       
       new G4PVPlacement(0, G4ThreeVector(x,y,z), db_logic,
			 "DB", _gas_logic, false, db_no, true);
       std::vector<std::pair<int, G4ThreeVector> > positions = db.GetPositions();
       for (G4int si=0; si<positions.size(); si++) {
	 G4ThreeVector mypos = positions[si].second;
	 std::pair<int, G4ThreeVector> abs_pos;
	 abs_pos.first = db_no*1000+positions[si].first ;
	 abs_pos.second = G4ThreeVector(x+mypos.getX(), y+mypos.getY(), z+mypos.getZ()); 
	 _absSiPMpos.push_back(abs_pos);
       }
       db_no++;
     }
   }

   /* First Daughter Board configuration */
  // Next1ELDBO dbo44_geom(4,4);
  // Next1ELDBO dbo43_geom(4,3);
  // Next1ELDBO dbo42_geom(4,2);
   
  // G4LogicalVolume* dbo44_logic = dbo44_geom.GetLogicalVolume();
  // G4LogicalVolume* dbo43_logic = dbo43_geom.GetLogicalVolume();
  // G4LogicalVolume* dbo42_logic = dbo42_geom.GetLogicalVolume();

  // G4RotationMatrix* rotdbo = new G4RotationMatrix();
  // rotdbo->rotateY(pi);
  // G4ThreeVector vec = rotdbo->getAxis();
  // G4double angle = rotdbo->getDelta();

  // G4double dbo44_xsize = dbo44_geom.GetDimensions().x();
  // G4double dbo44_ysize = dbo44_geom.GetDimensions().y();
  // G4double dbo43_xsize = dbo43_geom.GetDimensions().x();
  // G4double dbo43_ysize = dbo43_geom.GetDimensions().y();
  // G4double dbo42_xsize = dbo42_geom.GetDimensions().x();
  // G4double dbo42_ysize = dbo42_geom.GetDimensions().y();

  // // The z dimension is the same for every kind of board
  // G4double dbo_zsize = dbo44_geom.GetDimensions().z();
     /// Distance of SiPM surface from the beginning of EL region
   // G4double dist_el = 2.5*mm;
   // G4double z = _elgap_position.z() + _elgap_length/2. + dist_el 
   //   + dbo_zsize/2.; 
   // G4double gap = 2.*mm;
  // G4int dbo_no = 1;
  
  // for (G4int i=0; i<4; i++) {
  //   G4double y = 3.*gap/2. + 3./2.*dbo44_ysize - i * (dbo44_ysize+gap);
  //   for (G4int j=0; j<3; j++) {
  //     G4double x = - dbo44_xsize - gap + j * (dbo44_xsize+gap);
  //     new G4PVPlacement(rotdbo, G4ThreeVector(x,y,z), dbo44_logic,
  //  			"DBO44", _gas_logic, false, dbo_no, true);
  //     std::vector<std::pair<int, G4ThreeVector> > positions = dbo44_geom.GetPositions();
  //     for (G4int si=0; si<positions.size(); si++) {
  // 	G4ThreeVector mypos = positions[si].second;
  // 	mypos.rotate(angle, vec);
  // 	std::pair<int, G4ThreeVector> abs_pos;
  // 	abs_pos.first = dbo_no*1000+positions[si].first ;
  // 	abs_pos.second = G4ThreeVector(x+mypos.getX(), y+mypos.getY(), z+mypos.getZ()); 
  // 	_absSiPMpos.push_back(abs_pos);
  //     }
  //     dbo_no++;
  //   }
  // }

  // G4double x = - 3./2.*dbo44_xsize - 2.*gap - dbo42_xsize/2.;
  // G4double y = gap + dbo44_ysize;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,z), dbo42_logic,
  // 		    "DBO42", _gas_logic, false, 13, true);
  // std::vector<std::pair<int, G4ThreeVector> > positions = dbo42_geom.GetPositions();
  // for (G4int si=0; si<positions.size(); si++) {
  //   G4ThreeVector mypos = positions[si].second;
  //   mypos.rotate(angle, vec);
  //   std::pair<int, G4ThreeVector> abs_pos;
  //   abs_pos.first = 13*1000+positions[si].first ;
  //   abs_pos.second = G4ThreeVector(x+mypos.getX(), y+mypos.getY(), z+mypos.getZ()); 
  //   _absSiPMpos.push_back(abs_pos);
  // }
  // x = -x;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,z), dbo42_logic,
  // 		    "DBO42", _gas_logic, false, 14, true);
  // for (G4int si=0; si<positions.size(); si++) {
  //   G4ThreeVector mypos = positions[si].second;
  //   mypos.rotate(angle, vec);
  //   std::pair<int, G4ThreeVector> abs_pos;
  //   abs_pos.first = 14*1000+positions[si].first ;
  //   abs_pos.second = G4ThreeVector(x+mypos.getX(), y+mypos.getY(), z+mypos.getZ()); 
  //   _absSiPMpos.push_back(abs_pos);
  // }
  // y = -y;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,z), dbo42_logic,
  // 		    "DBO42", _gas_logic, false, 15, true);
  // for (G4int si=0; si<positions.size(); si++) {
  //   G4ThreeVector mypos = positions[si].second;
  //   mypos.rotate(angle, vec);
  //   std::pair<int, G4ThreeVector> abs_pos;
  //   abs_pos.first = 15*1000+positions[si].first ;
  //   abs_pos.second = G4ThreeVector(x+mypos.getX(), y+mypos.getY(), z+mypos.getZ()); 
  //   _absSiPMpos.push_back(abs_pos);
  // }
  // x = -x;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,z), dbo42_logic,
  // 		    "DBO42", _gas_logic, false, 16, true);
  // for (G4int si=0; si<positions.size(); si++) {
  //   G4ThreeVector mypos = positions[si].second;
  //   mypos.rotate(angle, vec);
  //   std::pair<int, G4ThreeVector> abs_pos;
  //   abs_pos.first = 16*1000+positions[si].first ;
  //   abs_pos.second = G4ThreeVector(x+mypos.getX(), y+mypos.getY(), z+mypos.getZ()); 
  //   _absSiPMpos.push_back(abs_pos);
  // }
  
  // x = - 3./2.*dbo44_xsize - 2.*gap - dbo43_xsize/2.;
  // y = 0;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,z), dbo43_logic,
  // 		    "DBO43", _gas_logic, false, 17, true);
  // positions = dbo43_geom.GetPositions();
  // for (G4int si=0; si<positions.size(); si++) {
  //   G4ThreeVector mypos = positions[si].second;
  //   mypos.rotate(angle, vec);
  //   std::pair<int, G4ThreeVector> abs_pos;
  //   abs_pos.first = 17*1000+positions[si].first ;
  //   abs_pos.second = G4ThreeVector(x+mypos.getX(), y+mypos.getY(), z+mypos.getZ()); 
  //   _absSiPMpos.push_back(abs_pos);
  // }
  // x = -x;
  // new G4PVPlacement(rotdbo, G4ThreeVector(x,y,z), dbo43_logic,
  // 		    "DBO43", _gas_logic, false, 18, true);
  // for (G4int si=0; si<positions.size(); si++) {
  //   G4ThreeVector mypos = positions[si].second;
  //   mypos.rotate(angle, vec);
  //   std::pair<int, G4ThreeVector> abs_pos;
  //   abs_pos.first = 18*1000+positions[si].first ;
  //   abs_pos.second = G4ThreeVector(x+mypos.getX(), y+mypos.getY(), z+mypos.getZ()); 
  //   _absSiPMpos.push_back(abs_pos);
  // }
}

void Next1EL::PrintAbsoluteSiPMPos()
{
  G4cout << "----- Absolute position of SiPMs in gas volume -----" << G4endl;
  G4cout <<  G4endl;
  for (G4int i=0; i<_absSiPMpos.size(); i++) {
    std::pair<int, G4ThreeVector> abs_pos = _absSiPMpos[i];
    G4cout << "ID number: " << _absSiPMpos[i].first << ", position: " 
    	   << _absSiPMpos[i].second.getX() << ", "
    	   << _absSiPMpos[i].second.getY() << ", "
    	   << _absSiPMpos[i].second.getZ()  << G4endl;
  }
  G4cout <<  G4endl;
  G4cout << "---------------------------------------------------" << G4endl;
}

G4ThreeVector Next1EL::GenerateVertex(const G4String& region) const
{
  if (region == "SIDEPORT") {
    return _sideport_position;
  }
  else if (region == "AXIALPORT") {
    return _axialport_position;
  }
  else if (region == "CENTER") {
    return _active_position;
  }
  else if (region == "ACTIVE") {
    return _hexrnd->GenerateVertex(INSIDE);
  } 
  else if (region == "EL_PLANE"){
    return _elgap_position;
  } 
  else if (region == "AD_HOC"){
    return _specific_vertex; 
  } 
  else if (region == "EL_TABLE") {
    G4ThreeVector vtx = _table_vertices[_idx_table];
    
    _idx_table++;
    if (_idx_table == _table_vertices.size()) {
      G4cout << "[Next1EL] Vertices table fully sampled already. "
	     << "Aborting the run..." << G4endl;
      G4RunManager::GetRunManager()->AbortRun();
    }
    
    return vtx;
  }
}



void Next1EL::CalculateELTableVertices(G4double binning)
{
  G4ThreeVector vertex;

  // We select the beginning of the EL gap as Z position
  vertex.setZ(_elgap_position.z() - _elgap_length/2.);

  // The vertices are created following a square grid with side
  // equal to the diameter of the EL gap. If a point of the grid 
  // falls outside the circumference defined by the EL rings,
  // it is rejected.
  
  const G4double radius = _elgap_ring_diam / 2.;
  const G4int imax = floor(2*radius/binning);
  
  for (G4int i=0; i<imax; i++) {
    vertex.setX(-radius + i*binning);
    
    for (int j=0; j<imax; j++) {
      vertex.setY(-radius + j*binning);
      
      if (sqrt(vertex.x()*vertex.x() + vertex.y()*vertex.y()) < radius)
	_table_vertices.push_back(vertex);
    }
  }

  // Check whether the job has enough events to fully sample the 
  // vertices table
  
  const ParamStore& cfgjob = ConfigService::Instance().Job();
  G4int num_events = cfgjob.GetIParam("number_events");
  
  if (num_events < _table_vertices.size()) {
    G4cout << "\n[Next1EL] ERROR.- "
	   << "Not enough events in job to scan completely\n the EL vertices table."
	   << " You'd need " << _table_vertices.size() << " events to do that."
	   << G4endl;
    
  }
}
