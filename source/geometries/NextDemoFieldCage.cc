// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <paola.ferrario@ific.uv.es>, <jmunoz@ific.uv.es>
//  Created: 1 Mar 2012
//
//  Copyright (c) 2012 NEXT Collaboration
//
//  Updated to NextDemo++  by  Ruth Weiss Babai <ruty.wb@gmail.com> 
//  Based on: NextNewFieldCage.cc and Next1EL.cc
//  Date:   June 2019
// ----------------------------------------------------------------------------

#include "NextDemoFieldCage.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "XenonGasProperties.h"
//#include "CylinderPointSampler.h"
//#include "HexagonPointSampler.h"
#include "DecagonPointSampler.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4UnitsTable.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  NextDemoFieldCage::NextDemoFieldCage(const G4double vessel_length):
  // NextDemoFieldCage::NextDemoFieldCage():

    BaseGeometry(),
    _mother_logic(nullptr), _mother_phys(nullptr),

  // ELECTROLUMINESCENCE GAP /////////////////////
  // Ruty:: changed according to drawing from:(0202-10 to 0202-25),(Prev. _elgap_length (5. * mm),)
  _elgap_length (10.8 * mm),
  // 0202-5-Poly	EL	Holder	Gate	side
  _elgap_ring_diam   (232. * mm),    // (prev: 229. * mm)
  _elgap_ring_thickn ( 29. * mm),    // (prev: 12. * mm)
  // Ruty:: changed according to drawing from:(0202-09) (Prev. _elgap_ring_height (5. * mm),)
  _elgap_ring_height ( 14.9 * mm),   // ( 22. *mm ; net length = 14.9 * mm) in STEP - file // It was taken twice
  // _wire_diam(.003048 * cm),
  _GateHV_Mesh_Dist ( 8.8 * mm),   // from STEP-file
  _anode_length ( 3. * mm),
  _anode_diam ( 256.3 * mm),     // from STEP-file
  // 0202-12-Poly	EL	Holder	TP	side
  _anode_ring_diam (256.4 * mm), // //should be 254*mm enlarge it to avoid overlap with anode plate// (prev: 229. * mm)
  _anode_ring_thickn ( 16.8 * mm),  // should be 18*mm make it smaller according to avoiding the overlap
  _anode_ring_length ( 13.1 * mm),  // ??? or (11.6*mm),
  _cathode_ring_height ( 8. * mm),
  _cathode_ring_diam   (229. * mm),
  _cathode_ring_thickn ( 10. * mm),
  // TPB
  _tpb_thickn (.001 * mm),
  // LIGHT TUBE //////////////////////////////////
  // _ltube_diam      (160. * mm),  // prev.
  _ltube_diam      (194.227 * mm),     // Neus: 194.97  ???  or 194.427  ???
  _ltube_thickn    ( 8.0 * mm + _tpb_thickn), // (prev:  5. * mm + _tpb_thickn),  (STEP file:  8. * mm)
  _ltube_up_length (288. * mm),  //  Ruty:: 0202-06 LT now->(288. * mm),??? (prev: 295. * mm) wide=61.0 mm
  _ltube_bt_length (103. * mm),  //  Ruty:: 0202-06 BT(prev: 100. * mm),
  _ltube_gap       ( 13.6 * mm),  // (prev:  3. * mm) or (  13.6 * mm) to match distances as in STEP-file ???
  _ltube_up_gap2cathd ( 7.5 * mm),
  _ltube_bt_gap2cathd ( 6.1 * mm),
  // ACTIVE VOLUME ///////////////////////////////
  _active_diam   (_ltube_diam),
  _active_length (301.9 * mm),  // from Drawing // (prev: 300. * mm)
  // FIXING RINGS /////////////////////////
  _fix_ring_diam   (235. * mm),  // from STEP-file
  _fix_ring_height ( 11. * mm),  //  - " " -  (13. or 11.) ???
  _fix_ring_thickn ( 22. * mm),  // from STEP-file
  // FIELD CAGE //////////////////////////////////
  _fieldcage_length ( _anode_length + _elgap_length + _GateHV_Mesh_Dist  //  _elgap_ring_height - out;anode_length - in ;
		     + _active_length + _ltube_bt_gap2cathd +    //  _ltube_gap 
		     _ltube_bt_length + _fix_ring_height/2.), //

  _fieldcage_displ  (69. *mm),  //  _fieldcage_displ  (prev:  84.*mm),
  // FIELD SHAPING RINGS /////////////////////////
  _ring_diam   (235. * mm),  // from STEP-file // (prev: 229. * mm),
  _ring_height ( 10. * mm),
  _ring_thickn (  5. * mm),  //  ( 5. * mm) according to STEP ??? //  (prev:  6. * mm)
  _ring_up_bt_gap (37.2 * mm),  //  from STEP-file
  // SUPPORT BARS ////////////////////////////////
  _bar_length  ((285. + 8.) * mm),  //  (prev:  expression) // (293 * mm)
  _bar_width   ( 19. * mm),  //  (prev:  40. * mm)
  _bar_thickn  ( 11. * mm),  //  (prev:  10. * mm),  (STEP file:  11. * mm)
  _bar_addon1_length ( 7. * mm),  // (prev:  none)   (STEP file:   8. * mm) or ( 7. * mm)  ?
  _bar_addon1_thickn (16. * mm),  // (prev:  ),
  _bar_addon2_length (16.5 * mm),  // (prev:  50. * mm), (16.5 * mm) + ( 7. * mm) = tot: 23.5*mm
  _bar_addon2_thickn ( 7. * mm),
  _barBT_length ( 97.5 * mm),
  _barBT_addonBT_length ( 7.5 * mm),
  _barBT_addonBT_thickn ( 8.9 * mm),
  //////////////////////////////////////////////////
  // VESSEL
  // _vessel_length (600. * mm),
  // DEFAULT VALUES FOR SOME PARAMETERS///////////
  //_temperature(300.* kelvin),  // _temperature <<==>> 303 ; 4 places here

  _max_step_size(1.*mm),
  _tpb_coating(true),

    _visibility (0),
    _verbosity(0),

    // EL field ON or OFF
    _elfield(0),
    _el_table_index(0),
    _el_table_binning(5.*mm)

  {

   /// Needed External variables
   _vessel_length = vessel_length;

    // Define a new category
    new G4UnitDefinition("kilovolt/cm","kV/cm","Electric field", kilovolt/cm);
    new G4UnitDefinition("mm/sqrt(cm)","mm/sqrt(cm)","Diffusion", mm/sqrt(cm));

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextDemo/", "Control commands of geometry NextDemo.");
    _msg->DeclareProperty("field_cage_vis", _visibility, "Field Cage Visibility");
    _msg->DeclareProperty("field_cage_verbosity", _verbosity, "Field Cage verbosity");

  G4GenericMessenger::Command& max_step_size_cmd =
    _msg->DeclareProperty("max_step_size", _max_step_size, "Set maximum step size.");
  max_step_size_cmd.SetUnitCategory("Length");
  max_step_size_cmd.SetParameterName("max_step_size", true);
  max_step_size_cmd.SetRange("max_step_size>0");

    // Boolean-type properties (true or false)
   _msg->DeclareProperty("elfield", _elfield, "True if the EL field is on (full simulation), false if it's not (parametrized simulation).");

  _msg->DeclareProperty("tpb_coating", _tpb_coating,
			"True if the upper light tube is coated.");

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

  G4GenericMessenger::Command& gate_transparency_cmd =
    _msg->DeclareProperty("gate_transparency", _gate_transparency,
			  "Set the transparency of the gate EL mesh.");
  gate_transparency_cmd.SetParameterName("gate_transparency", false);
  gate_transparency_cmd.SetRange("gate_transparency>0 && gate_transparency<1");

  }

  // void NextDemoFieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)  // Ruty - like Next100
  void NextDemoFieldCage::SetLogicalVolume(G4LogicalVolume* mother_logic)          /////  Ruty - from New    /////
  {
    _mother_logic = mother_logic;
    _gas = _mother_logic->GetMaterial();
    _pressure =    _gas->GetPressure();
    _temperature = _gas->GetTemperature();
  }



  void NextDemoFieldCage::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    _mother_phys = mother_phys;
  }



  void NextDemoFieldCage::Construct()
  {
      // Store the position of the center of the fieldcage
      // RUTY from Next1EL.cc
      // VESSEL :: // _vessel_length (600. * mm);( _vessel_length/2. = 300*mm)
  _fieldcage_position.
    set(0., 0., (_vessel_length/2. -_fieldcage_length/2.-_fieldcage_displ));  // (0.,0.,11.45)*mm
    //set(0., 0., (300.*mm -_fieldcage_length/2.-_fieldcage_displ));  // (prev: 0.,0.,9.5)*mm

   G4double diel_thickn = .1*mm;

  // Position of the electrodes in the fieldcage
  _anode_posz   = _fieldcage_length/2.  - _anode_length/2.; // (prev: - _elgap_ring_height)
  _gate_posz    = _anode_posz - _anode_length/2. - _elgap_length ;   // It is the MESH posz not the GateHV posz // The mesh itself is in the EL_gap 0.1*mm //
  _cathode_posz = _gate_posz  - _GateHV_Mesh_Dist - _active_length;
  // World==Vessel Coordinates => EL_table_gate_posz = _gate_posz + _fieldcage_position.z
  _el_table_z = _gate_posz + _fieldcage_position.z();
   // 0.1 * mm is added to avoid very small negative numbers in drift lengths
  SetELzCoord(_el_table_z + 0.1 * mm); // Gate-Mesh Posz
 ////////////////////////
  _AnodezCoord = _el_table_z  + _elgap_length + _anode_length/2.;  // To the Edge of EL_GAP or before the Anode, facing Sipm (+ _anode_length/2.) ??
 /////////////////////
   /// Ionielectrons are generated at a z = .5 mm inside the EL gap, not on border
  _el_table_z = _el_table_z + 0.5*mm;    // Next1EL.cc  SEE down here from: NextNewFieldCage.cc !!!
  //_el_table_z = _el_table_z + 0.2*mm;       // NextNewFieldCage.cc
   /*
   // For EL Table generation
   _idx_table = 0;
  _table_vertices.clear();

   G4double z = _vessel_length/2. - _fieldcage_displ - _elgap_ring_height - _elgap_length;
   SetELzCoord(z);
   z = z +  .5*mm;
   CalculateELTableVertices(92.5*mm, 5.*mm, z);
   */

    // From::  NextNewFieldCage.cc
    G4double max_radius = floor(_ltube_diam/2./_el_table_binning)*_el_table_binning;
    //   _active_diam   (_ltube_diam),
    CalculateELTableVertices(max_radius, _el_table_binning, _el_table_z);
   // CalculateELTableVertices(92.5*mm, 5.*mm, z);    // Next1EL.cc

    // for (G4int i=0; i<_el_table_vertices.size(); ++i) {
    //   std::cout << i << ": "<< _el_table_vertices[i] << std::endl;
    // }

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

 // DefineMaterials()
  // ALUMINUM
  _aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
  // PTFE (TEFLON)
  _teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  //TPB + +
  _tpb = MaterialsList::TPB();
  _tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
  // Quartz
  _quartz =  MaterialsList::FusedSilica();
  _quartz->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());


  // ELECTROLUMINESCENCE GAP /////////////////////////////////////////

    // GATE AND ANODE (EL) RINGS ..... Modify due to STEP file ................

  G4Tubs* anode_ring_solid =
    new G4Tubs("ANODE_RING", _anode_ring_diam/2.,
	       (_anode_ring_diam/2. + _anode_ring_thickn),
	       _anode_ring_length/2., 0, twopi);

  G4LogicalVolume* anode_ring_logic =
    new G4LogicalVolume(anode_ring_solid, _aluminum, "ANODE_RING");

  G4double posz = _fieldcage_length/2. ; // STEP file (? - _anode_length), (prev: - _elgap_ring_height/2.)

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), anode_ring_logic,
   		    "ANODE_RING", _mother_logic, false, 0, true);

   if (_verbosity) {
      G4cout << "Anode ring starts in " 
             << posz + _fieldcage_position.z() - _anode_ring_length/2. << G4endl
             << " and ends in " 
             << posz + _fieldcage_position.z() + _anode_ring_length/2. 
             << G4endl;
   }
//////////////////////////////////////////////////////////////////////////////
  G4Tubs* elgap_ring_solid =
    new G4Tubs("EL_GAP_RING", _elgap_ring_diam/2.,
	       (_elgap_ring_diam/2. + _elgap_ring_thickn),
	       _elgap_ring_height/2., 0, twopi);

  G4LogicalVolume* elgap_ring_logic =
    new G4LogicalVolume(elgap_ring_solid, _aluminum, "EL_GAP_RING");

  // posz = _fieldcage_length/2. - _anode_ring_length/2. - 5.5 * mm - _elgap_ring_height/2.; 
  posz = posz - _anode_ring_length/2. - 5.5 * mm - _elgap_ring_height/2.;  // (1.1+4.4)*mm for distance, STEP file

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), elgap_ring_logic,
   		    "EL_GAP_RING", _mother_logic, false, 0, true);

   if (_verbosity) {
      G4cout << "Gate ring starts in " 
             << posz + _fieldcage_position.z() - _elgap_ring_height/2. << G4endl
             << " and ends in " 
             << posz + _fieldcage_position.z() + _elgap_ring_height/2. 
             << G4endl;
   }


  if (_visibility) {
  G4VisAttributes * visEL = new G4VisAttributes;
  visEL->SetColor(0.2, 0.7, 0.3);
  visEL->SetForceWireframe(true);
  elgap_ring_logic->SetVisAttributes(visEL);
  }
  else {
    elgap_ring_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }

  // EL GAP ................................................

  G4Tubs* elgap_solid = new G4Tubs("EL_GAP", 0., _elgap_ring_diam/2.,
   				   _elgap_length/2., 0, twopi);

  G4LogicalVolume* elgap_logic =
    new G4LogicalVolume(elgap_solid, _gas, "EL_GAP");   // _gxe

  posz = _fieldcage_length/2. - _anode_length - _elgap_length/2.;  // (STEP file:- _anode_length),  (prev: - _elgap_ring_height)

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), elgap_logic,
		    "EL_GAP", _mother_logic, false, 0, true);

  // Store the position of the EL GAP wrt the WORLD system of reference
  _elgap_position.set(0.,0.,_fieldcage_position.z()+posz);

   if (_verbosity) {
     G4cout << "  ****************************  " << G4endl;
     G4cout << "*** Positions of el_gap FC coordinates***" << G4endl;
     G4cout << "posz: " << posz << G4endl;
     G4cout << "Positions of el_gap world coordinate: " << posz + _fieldcage_position.z() << G4endl;
     G4cout << "_elgap_position() " << _elgap_position.z() << G4endl;
     G4cout << "EL GAP starts in " << posz + _fieldcage_position.z() - _elgap_length/2. << G4endl
            << " and ends in " <<  posz + _fieldcage_position.z() + _elgap_length/2. << G4endl;
     G4cout << "_fieldcage_position: " <<  _fieldcage_position << G4endl;
     G4cout << "_fieldcage_length: " <<  _fieldcage_length << G4endl;
     G4cout << "_anode_position (wrld): " <<  _anode_posz + _fieldcage_position.z() << G4endl;
     G4cout << "_cathode_position (wrld): " <<  _cathode_posz + _fieldcage_position.z() << G4endl;
     G4cout << "  ****************************  " << G4endl;
   }
  //////////////////////////////////////////////////////////////////////////////////


  // EL GRIDS

  // G4double diel_thickn = .1*mm;


  G4Material* fgrid_gate = MaterialsList::FakeDielectric(_gas, "grid_mat");
  fgrid_gate->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature,
									_gate_transparency, diel_thickn));

  G4Tubs* diel_grid =
    new G4Tubs("GRID_GATE", 0., _elgap_ring_diam/2., diel_thickn/2., 0, twopi);

  G4LogicalVolume* diel_grid_gate_logic =
    new G4LogicalVolume(diel_grid, fgrid_gate, "GRID_GATE");
  G4double pos1 = - _elgap_length/2. + diel_thickn/2.;
  new G4PVPlacement(0, G4ThreeVector(0.,0.,pos1), diel_grid_gate_logic, "GRID_GATE",
		    elgap_logic, false, 0, true);


 //G4double anode_diam = _anode_quartz_diam;

    ///// ANODE //////

    G4Tubs* anode_quartz_solid =
      new G4Tubs("EL_QUARTZ_ANODE", 0., _anode_diam/2. , _anode_length/2., 0, twopi);

    G4LogicalVolume* anode_logic =
      new G4LogicalVolume(anode_quartz_solid, _quartz, "EL_QUARTZ_ANODE");

    G4double _pos_z_anode = _anode_posz + _fieldcage_position.z();

    new G4PVPlacement(0, G4ThreeVector(0., 0., _pos_z_anode), anode_logic,
		      "EL_QUARTZ_ANODE", _mother_logic, false, 0, true);

   if (_verbosity) {
       G4cout << "Anode plate starts in " << _pos_z_anode - _anode_length/2. << G4endl
              << " and ends in "          << _pos_z_anode + _anode_length/2. << G4endl;
   }


  if (_elfield) {
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    el_field->SetCathodePosition(_elgap_position.z()-_elgap_length/2.);
    el_field->SetAnodePosition(_elgap_position.z()+_elgap_length/2.);
    el_field->SetDriftVelocity(2.5*mm/microsecond);
    el_field->SetTransverseDiffusion(1.*mm/sqrt(cm));
    el_field->SetLongitudinalDiffusion(.5*mm/sqrt(cm));
    XenonGasProperties xgp(_pressure, _temperature);
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
    // new G4Polyhedra("ACTIVE", 0., twopi, 6, 2, zplane, rinner, router);
    new G4Polyhedra("ACTIVE", 0., twopi, 10, 2, zplane, rinner, router);

  G4LogicalVolume* active_logic =
    new G4LogicalVolume(active_solid, _gas, "ACTIVE");

  //  posz = _fieldcage_length/2. - _elgap_ring_height - _elgap_length - _active_length/2.;  Prev:
      posz = _fieldcage_length/2.  - _anode_length - _elgap_length - _GateHV_Mesh_Dist - _active_length/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), active_logic,
		    "ACTIVE", _mother_logic, false, 0, true);

  // Store the position of the active volume with respect to the
  // WORLD system of reference
  _active_position.set(0.,0.,_fieldcage_position.z()+posz);

  //_hexrnd = new HexagonPointSampler(_active_diam/2., _active_length, 0.,
				  //  _active_position);

  // Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(_max_step_size));

  // Set the volume as an ionization sensitive detector
  // G4String det_name = "/DEMO/ACTIVE";
  // IonizationSD* ionisd = new IonizationSD(det_name);
  IonizationSD* ionisd = new IonizationSD("/DEMO/ACTIVE");
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
    // new G4Polyhedra("LIGHT_TUBE_UP", 0., twopi, 6, 2, zplane, rinner, router);
    new G4Polyhedra("LIGHT_TUBE_UP", 0., twopi, 10, 2, zplane, rinner, router);

  G4LogicalVolume* ltube_up_logic =
    new G4LogicalVolume(ltube_up_solid, _teflon, "LIGHT_TUBE_UP");

  //  posz = (_gate_posz + _cathode_posz) / 2.;  Prev.
  posz = (_gate_posz - _GateHV_Mesh_Dist + _cathode_posz) / 2.;  // STEP file: ( - _GateHV_Mesh_Dist)

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), ltube_up_logic,
   		    "LIGHT_TUBE_UP", _mother_logic, false, 0, true);

  if (_verbosity) {
	G4cout << "  ****************************  " << G4endl;
        G4cout << "*** Positions of internal upper light tubs ***" << G4endl;
	G4cout << "posz: " << posz << G4endl;
	G4cout << "_fieldcage_position.z() : "  <<  _fieldcage_position.z() << G4endl;
	G4cout << "LIGHT_TUBE_UP.Z: " << posz + _fieldcage_position.z() << G4endl;
	G4cout << "LIGHT_TUBE_UP.Z edge: " << posz + _fieldcage_position.z() + _ltube_up_length/2. << G4endl;
	G4cout << "  ****************************  " << G4endl;
  }

  // TPB coating
  //if (_tpb_coating) {
    G4double rinner_tpb[2];
    G4double router_tpb[2];
    rinner_tpb[0] = _ltube_diam/2.; rinner_tpb[1] = rinner_tpb[0];
    router_tpb[0] = _ltube_diam/2. + _tpb_thickn; router_tpb[1] = router_tpb[0];

    G4Polyhedra* ltube_tpb_solid =
      //  new G4Polyhedra("LIGHT_TUBE_TPB", 0., twopi, 6, 2, zplane,
      new G4Polyhedra("LIGHT_TUBE_TPB", 0., twopi, 10, 2, zplane,
		      rinner_tpb, router_tpb);
    G4LogicalVolume* ltube_tpb_logic =
      new G4LogicalVolume(ltube_tpb_solid, _tpb, "LIGHT_TUBE_TPB");

    G4VPhysicalVolume* ltube_tpb_phys =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), ltube_tpb_logic,
		      "LIGHT_TUBE_TPB", ltube_up_logic, false, 0, true);

    /*G4VisAttributes * visattrib_red = new G4VisAttributes;
    visattrib_red->SetColor(1., 0., 0.);
    //visattrib_red->SetForceWireframe(true);  //
    visattrib_red->SetForceSolid(true);
    ltube_tpb_logic->SetVisAttributes(visattrib_red);*/
    
  //}

  // BOTTOM PART ...........................................

  zplane[0] = -_ltube_bt_length/2.; zplane[1] = _ltube_bt_length/2.;

  G4Polyhedra* ltube_bt_solid =
    // new G4Polyhedra("LIGHT_TUBE_BOTTOM", 0., twopi, 6, 2, zplane, rinner, router);
    new G4Polyhedra("LIGHT_TUBE_BOTTOM", 0., twopi, 10, 2, zplane, rinner, router);

  G4LogicalVolume* ltube_bt_logic =
    new G4LogicalVolume(ltube_bt_solid, _teflon, "LIGHT_TUBE_BOTTOM");

  // posz = _fieldcage_length/2. - _elgap_ring_height - _elgap_length -   // Prev.
  //        _active_length - _ltube_gap - _ltube_bt_length/2.;
  posz = (_gate_posz - _GateHV_Mesh_Dist + _cathode_posz) / 2.     // STEP file: ( - _GateHV_Mesh_Dist)
         -  _ltube_up_length/2.  - _ltube_gap  - _ltube_bt_length/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), ltube_bt_logic,
   		    "LIGHT_TUBE_BOTTOM", _mother_logic, false, 0, true);

///////////////////////////////////////////////////////////////////////////////////
   if (_verbosity) {
	   G4cout << "  ****************************  " << G4endl;
	   G4cout << "*** Positions of internal light tubs ***" << G4endl;
	   G4cout << "posz: " << posz << G4endl;
	   G4cout << "LIGHT_TUBE_BOTTOM.Z: " << posz + _fieldcage_position.z() << G4endl;
	   G4cout << "_fieldcage_position: " <<  _fieldcage_position << G4endl;
	   G4cout << "ACTIVE: " << _active_position << G4endl;
	   G4cout << "  ****************************  " << G4endl;
   }
//////////////////////////////////////////////////////////////////////////////////

  // SETTING VISIBILITIES   //////////
  if (_visibility) {
    G4VisAttributes titanium_col = nexus::TitaniumGrey();
    titanium_col.SetForceSolid(true);
    ltube_up_logic->SetVisAttributes(titanium_col);
    ltube_bt_logic->SetVisAttributes(titanium_col);
   } else {
       ltube_up_logic->SetVisAttributes(G4VisAttributes::Invisible);
       ltube_bt_logic->SetVisAttributes(G4VisAttributes::Invisible);
   }

//////////////////////////////////////////////////////////////////////////////////

  // FIELD SHAPING RINGS /////////////////////////////////////////////

  G4Tubs* ring_solid = new G4Tubs("FIELD_RING", _ring_diam/2.,
   				  (_ring_diam/2.+_ring_thickn),
  				  _ring_height/2., 0, twopi);

  G4LogicalVolume* ring_logic =
    new G4LogicalVolume(ring_solid, _aluminum, "FIELD_RING");


  // // DRIFT REGION ................................
   if (_verbosity) {
       G4cout << "  ***** RINGS DRIFT REGION* ****  " << G4endl;
       G4cout << "*** Positions  Z: ***" << G4endl;
   }

  G4int num_rings = 19;

  posz = _fieldcage_length/2. - _anode_length - _elgap_length
        - _GateHV_Mesh_Dist *2. - _ring_height/2.;  //  (- 10.*mm + 1.2 *mm) ?

  // RUTY ADD::    " _fieldcage_position.z() "  to  posZ

  for (G4int i=0; i<num_rings; i++) {
      if (_verbosity) {
         G4cout << "posz (wrld): " <<  i  << " :  " <<  posz  + _fieldcage_position.z() << G4endl;
      }
    new G4PVPlacement(0, G4ThreeVector(0., 0., posz + _fieldcage_position.z()), ring_logic,
                "FIELD_RING", _mother_logic, false, i, true);

    //  prev:  posz = posz - _ring_height - 5.1 * mm;
    posz = posz - _ring_height - 5.0 * mm;

  }

  // // BUFFER ......................................
   if (_verbosity) {
       G4cout << "  ***** RINGS BUFFER REGION* ****  " << G4endl;
       G4cout << "*** Positions  Z: ***" << G4endl;
   }

  posz = posz + _ring_height + 5.0 * mm - _ring_up_bt_gap;

  for (G4int i=19; i<23; i++) {
     if (_verbosity) {

        G4cout << "posz (wrld): "  <<  i  << " :  " << posz  + _fieldcage_position.z() << G4endl;
     }

    new G4PVPlacement(0, G4ThreeVector(0., 0., posz  + _fieldcage_position.z()), ring_logic,
                "FIELD_RING", _mother_logic, false, i, true);

    posz = posz - _ring_height - 10. * mm;

  }

  // SETTING VISIBILITIES   //////////
  //_visibility = 1;
  if (_visibility) {
    G4VisAttributes yellow_col =nexus::Yellow();
    yellow_col.SetForceSolid(true);
    ring_logic->SetVisAttributes(yellow_col);
   }
   else {
    ring_logic->SetVisAttributes(G4VisAttributes::Invisible);
   }
  //_visibility = 0;
  // CATHODE .....................................

  diel_thickn = 1. * mm;
  G4double transparency = 0.98;

  G4Material* fcathode = MaterialsList::FakeDielectric(_gas, "cathode_mat");
  fcathode->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature,
  									transparency, diel_thickn));

  G4Tubs* diel_cathd =
    new G4Tubs("CATHODE", 0., _cathode_ring_diam/2., diel_thickn/2., 0, twopi);  // _elgap_ring_diam/2

  G4LogicalVolume* diel_cathd_logic =
    new G4LogicalVolume(diel_cathd, fcathode, "CATHODE");

  posz = _cathode_posz - diel_thickn/2.;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + _fieldcage_position.z()), diel_cathd_logic, "CATHODE",
		    _mother_logic, false, 0, true);


  // SUPPORT BARS - DRIFT ////////////////////////////////////////////////////

  G4Box* bar_base = new G4Box("SUPPORT_BAR", _bar_thickn/2.,
			 _bar_width/2., _bar_length/2.);

  G4Box* addon1 = new G4Box("SUPPORT_BAR", _bar_addon1_thickn/2.,
			   _bar_width/2., _bar_addon1_length/2.);

  G4Box* addon2 = new G4Box("SUPPORT_BAR", _bar_addon2_thickn/2.,
			   _bar_width/2., _bar_addon2_length/2.);

  //  UNIONS for support bar (pillar) and add_on

  G4UnionSolid* bar_solid =
    new G4UnionSolid("SUPPORT_BAR", bar_base, addon1, 0,
		     G4ThreeVector((_bar_thickn + _bar_addon1_thickn)/2., 0.,
                                   -(_bar_length - _bar_addon1_length)/2.)); // + or -

  bar_solid =
    new G4UnionSolid("SUPPORT_BAR", bar_solid, addon2, 0,
                  G4ThreeVector(_bar_thickn/2. + _bar_addon1_thickn - _bar_addon2_thickn/2., 0,
                                -(_bar_length + _bar_addon2_length)/2.));


  G4LogicalVolume* bar_logic =
    new G4LogicalVolume(bar_solid, MaterialsList::PEEK(), "SUPPORT_BAR");
    //new G4LogicalVolume(bar_base, MaterialsList::PEEK(), "SUPPORT_BAR");

  G4double pos_rad = _ltube_diam/2. + _ltube_thickn + _bar_thickn/2.;
  posz = _fieldcage_length/2. - _anode_ring_length/2. - 5.6 * mm - _elgap_ring_height // - ;// (5.5*mm OR _elgap_length/2.)
  //posz = _fieldcage_length/2. - 2.*_elgap_ring_height - _elgap_length // prev.
       - _bar_length/2.;

   if (_verbosity) {
       G4cout << "  ******  SUPPORT_BAR  ******  " << G4endl;
       G4cout << "SUPPORT_BAR.Z: " << posz + _fieldcage_position.z() << G4endl;
       G4cout << "SUPPORT_BAR.Z up: " << posz + _fieldcage_position.z() + _bar_length/2. << G4endl;
       G4cout << "SUPPORT_BAR.Z bt: " << posz + _fieldcage_position.z() - _bar_length/2. << G4endl;
       G4cout << "  **************   " << G4endl;
   }

  // for (G4int i=0; i<6; i++) {
  for (G4int i=0; i<10; i++) {

    G4RotationMatrix rotbar;
    // rotbar.rotateZ(i*60.*deg);     // 360/6.
    rotbar.rotateZ((i*36.+90)*deg);     // 360/10.

    // G4double xx = pos_rad * cos(i*60.*deg);
    // G4double yy = pos_rad * sin(i*60.*deg);
    G4double xx = pos_rad * cos((i*36.+90)*deg);
    G4double yy = pos_rad * sin((i*36.+90)*deg);
    G4double zz = posz;

    new G4PVPlacement(G4Transform3D(rotbar, G4ThreeVector(xx, yy, zz + _fieldcage_position.z())),
		      bar_logic, "SUPPORT_BAR", _mother_logic, false, i, true);
  }
   /*G4VisAttributes * vis3 = new G4VisAttributes;
   vis3->SetColor(0.5, 0.5, .5);
   vis3->SetForceSolid(true);
   bar_logic->SetVisAttributes(vis3);*/

   if (_visibility) {
   G4VisAttributes dirty_white_col =nexus::DirtyWhite();
   dirty_white_col.SetForceSolid(true);
   bar_logic->SetVisAttributes(dirty_white_col);
   }
   else {
     bar_logic->SetVisAttributes(G4VisAttributes::Invisible);
   }

  // SUPPORT BARS - BUFFER //////////////////////////////////////////////

  G4Box* barBT_base = new G4Box("SUPPORT_BAR_BT", _bar_thickn/2.,
			 _bar_width/2., _barBT_length/2.);

  G4Box* addonBT = new G4Box("SUPPORT_BAR_BT", _barBT_addonBT_thickn/2.,
			   _bar_width/2., _barBT_addonBT_length/2.);

  G4UnionSolid* barBT_solid =
    new G4UnionSolid("SUPPORT_BAR_BT", barBT_base, addonBT, 0,
		     G4ThreeVector((_bar_thickn + _barBT_addonBT_thickn)/2., 0.,
                                   (_barBT_length - _barBT_addonBT_length)/2.));

  G4LogicalVolume* barBT_logic =
    new G4LogicalVolume(barBT_solid, MaterialsList::PEEK(), "SUPPORT_BAR_BT");

  posz = (_gate_posz - _GateHV_Mesh_Dist + _cathode_posz) / 2.     // STEP file: ( - _GateHV_Mesh_Dist)
         -  _ltube_up_length/2.  - _ltube_gap  - _ltube_bt_length/2. + 4.6 * mm; // poszBT + 4.35 * mm

   if (_verbosity) {
	  G4cout << "  ******  SUPPORT_BAR BT  ******  " << G4endl;
	  G4cout << "SUPPORT_BAR_BT.Z: " << posz + _fieldcage_position.z() << G4endl;
	  G4cout << "SUPPORT_BAR_BT.Z up: " << posz + _fieldcage_position.z() + _barBT_length/2. << G4endl;
	  G4cout << "SUPPORT_BAR_BT.Z bt: " << posz + _fieldcage_position.z() - _barBT_length/2. << G4endl;
	  G4cout << "  **************   " << G4endl;
	  G4cout << " _gate_transparency: " <<  _gate_transparency  << G4endl;
	  G4cout << "  _max_step_size  : " <<    _max_step_size     << G4endl;
	  G4cout << "  **************   " << G4endl;
   }

  // for (G4int i=0; i<6; i++) {
  for (G4int i=0; i<10; i++) {

    G4RotationMatrix rotbar;
    // rotbar.rotateZ(i*60.*deg);     // 360/6.
    rotbar.rotateZ((i*36.+90)*deg);     // 360/10.

    // G4double xx = pos_rad * cos(i*60.*deg);
    // G4double yy = pos_rad * sin(i*60.*deg);
    G4double xx = pos_rad * cos((i*36.+90)*deg); // xx
    G4double yy = pos_rad * sin((i*36.+90)*deg); // yy
    G4double zz = posz;

    new G4PVPlacement(G4Transform3D(rotbar, G4ThreeVector(xx, yy, zz + _fieldcage_position.z())),
		      barBT_logic, "SUPPORT_BAR_BT", _mother_logic, false, i, true);
  }


  if (_visibility) {
   G4VisAttributes dirty_white_col =nexus::DirtyWhite();
   dirty_white_col.SetForceSolid(true);
   barBT_logic->SetVisAttributes(dirty_white_col);
   }
   else {
     barBT_logic->SetVisAttributes(G4VisAttributes::Invisible);
   }

  ////////////////////////////////////////////////////////////////////
  // OPTICAL SURFACES ////////////////////////////////////////////////

  G4OpticalSurface* ltubeup_opsur = new G4OpticalSurface("LIGHT_TUBE_UP");
  ltubeup_opsur->SetType(dielectric_metal);
  ltubeup_opsur->SetModel(unified);
  ltubeup_opsur->SetFinish(ground);
  ltubeup_opsur->SetSigmaAlpha(0.1);  // (0.01 in NextNew)
  // new "OpticalMaterialProperties" approach based on NextNew:
  ltubeup_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
  new G4LogicalSkinSurface("LIGHT_TUBE_UP", ltube_up_logic, ltubeup_opsur);

  // Optical surface between gas and TPB to model the latter's roughness
  if (_tpb_coating) {
    //ltubeup_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_with_TPB());
  //} else {
   // ltubeup_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

    G4OpticalSurface* gas_tpb_teflon_surf =
      new G4OpticalSurface("GAS_TPB_TEFLON_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);

    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", ltube_tpb_phys, _mother_phys,
                               gas_tpb_teflon_surf);
    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", _mother_phys, ltube_tpb_phys,
                               gas_tpb_teflon_surf);
  }


  G4OpticalSurface* ltubebt_opsur = new G4OpticalSurface("LIGHT_TUBE_BOTTOM");
  ltubebt_opsur->SetType(dielectric_metal);
  ltubebt_opsur->SetModel(unified);
  ltubebt_opsur->SetFinish(ground);
  ltubebt_opsur->SetSigmaAlpha(0.1);
  ltubebt_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
  new G4LogicalSkinSurface("LIGHT_TUBE_BOTTOM", ltube_bt_logic, ltubebt_opsur);

// }

  /* G4VisAttributes dark_green_tub_up = nexus::DarkGreen();
    ltube_up_logic->SetVisAttributes(dark_green_tub_up);
  G4VisAttributes light_brown_tub_bt = nexus::CopperBrown();
    light_brown_tub_bt.SetForceSolid(true);
    ltube_bt_logic->SetVisAttributes(light_brown_tub_bt);
  */

////////////////////////////////////////////////////////////////////////////////////


    // VERTEX GENERATORS   //////////



        //_hexrnd = new HexagonPointSampler(_active_diam/2., _active_length, 0.,
				    //_active_position);

     _decrnd_gen = new DecagonPointSampler(_active_diam/2., _active_length, 0., _active_position);


  }



  NextDemoFieldCage::~NextDemoFieldCage()
  {
    //delete _hexrnd;
    delete _decrnd_gen;
  }



  G4ThreeVector NextDemoFieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    
     if (region == "ACTIVE") {
      //vertex = _hexrnd_gen->GenerateVertex(INSIDE);
      //vertex = _hexrnd_gen->GenerateVertex(PLANE);
      vertex = _decrnd_gen->GenerateVertex(INSIDE10);
    }
    else if (region == "AD_HOC") {
      vertex = G4ThreeVector(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
    }
    else if (region == "EL_TABLE") {

      unsigned int i = _el_table_point_id + _el_table_index;

      if (i == (_el_table_vertices.size()-1)) {
        G4Exception("[NextDemoFieldcage]", "GenerateVertex()",
		    RunMustBeAborted, "Reached last event in EL lookup table.");
      }

      try {
        vertex = _el_table_vertices.at(i);
        _el_table_index++;
      }
      catch (const std::out_of_range& oor) {
        G4Exception("[NextDemoFieldCage]", "GenerateVertex()", FatalErrorInArgument, "EL lookup table point out of range.");
      }
    }
    else {
      G4Exception("[NextDemoFieldCage]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
    //*/
  }


  void NextDemoFieldCage::CalculateELTableVertices(G4double radius, G4double binning, G4double z)
  {
    // Calculate the xyz positions of the points of an EL lookup table
    // (arranged as a square grid) given a certain binning

    G4ThreeVector xyz(0.,0.,z);

    G4int imax = floor(2.*radius/binning); // max bin number (minus 1)

    for (G4int i=0; i<imax+1; ++i) { // Loop through the x bins

      xyz.setX(-radius + i*binning); // x position

      for (G4int j=0; j<imax+1; ++j) { // Loop through the y bins

        xyz.setY(-radius + j*binning); // y position

        // Store the point if it's inside the active volume defined by the
        // field cage (of circular cross section). Discard it otherwise.
        if (sqrt(xyz.x()*xyz.x()+xyz.y()*xyz.y()) <= radius)
          _el_table_vertices.push_back(xyz);
      }
    }
  }



  G4ThreeVector NextDemoFieldCage::GetPosition() const
  {
    return G4ThreeVector (0., 0., _fieldcage_position.z());
  }

  G4double NextDemoFieldCage::GetAnodezCoord() const
  {
    return _AnodezCoord;
  }


}
