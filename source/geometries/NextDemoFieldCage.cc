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
    mother_logic_(nullptr), mother_phys_(nullptr),

  // ELECTROLUMINESCENCE GAP /////////////////////
  // Ruty:: changed according to drawing from:(0202-10 to 0202-25),(Prev. _elgap_length (5. * mm),)
  elgap_length_ (10.8 * mm),
  // 0202-5-Poly	EL	Holder	Gate	side
  elgap_ring_diam_   (232. * mm),    // (prev: 229. * mm)
  elgap_ring_thickn_ ( 29. * mm),    // (prev: 12. * mm)
  // Ruty:: changed according to drawing from:(0202-09) (Prev. _elgap_ring_height (5. * mm),)
  elgap_ring_height_ ( 14.9 * mm),   // ( 22. *mm ; net length = 14.9 * mm) in STEP - file // It was taken twice
  // _wire_diam(.003048 * cm),
  GateHV_Mesh_Dist_ ( 8.8 * mm),   // from STEP-file
  anode_length_ ( 3. * mm),
  anode_diam_ ( 256.3 * mm),     // from STEP-file
  // 0202-12-Poly	EL	Holder	TP	side
  anode_ring_diam_ (256.4 * mm), // //should be 254*mm enlarge it to avoid overlap with anode plate// (prev: 229. * mm)
  anode_ring_thickn_ ( 16.8 * mm),  // should be 18*mm make it smaller according to avoiding the overlap
  anode_ring_length_ ( 13.1 * mm),  // ??? or (11.6*mm),
  cathode_ring_height_ ( 8. * mm),
  cathode_ring_diam_   (229. * mm),
  cathode_ring_thickn_ ( 10. * mm),
  // TPB
  tpb_thickn_ (.001 * mm),
  // LIGHT TUBE //////////////////////////////////
  // _ltube_diam      (160. * mm),  // prev.
  ltube_diam_      (194.227 * mm),     // Neus: 194.97  ???  or 194.427  ???
  ltube_thickn_    ( 8.0 * mm + tpb_thickn_), // (prev:  5. * mm + tpb_thickn_),  (STEP file:  8. * mm)
  ltube_up_length_ (288. * mm),  //  Ruty:: 0202-06 LT now->(288. * mm),??? (prev: 295. * mm) wide=61.0 mm
  ltube_bt_length_ (103. * mm),  //  Ruty:: 0202-06 BT(prev: 100. * mm),
  ltube_gap_       ( 13.6 * mm),  // (prev:  3. * mm) or (  13.6 * mm) to match distances as in STEP-file ???
  ltube_up_gap2cathd_ ( 7.5 * mm),
  ltube_bt_gap2cathd_ ( 6.1 * mm),
  // ACTIVE VOLUME ///////////////////////////////
  active_diam_   (ltube_diam_),
  active_length_ (301.9 * mm),  // from Drawing // (prev: 300. * mm)
  // FIXING RINGS /////////////////////////
  fix_ring_diam_   (235. * mm),  // from STEP-file
  fix_ring_height_ ( 11. * mm),  //  - " " -  (13. or 11.) ???
  fix_ring_thickn_ ( 22. * mm),  // from STEP-file
  // FIELD CAGE //////////////////////////////////
  fieldcage_length_ ( anode_length_ + elgap_length_ + GateHV_Mesh_Dist_  //  elgap_ring_height_ - out;anode_length - in ;
		     + active_length_ + ltube_bt_gap2cathd_ +    //  ltube_gap_ 
		     ltube_bt_length_ + fix_ring_height_/2.), //

  fieldcage_displ_  (69. *mm),  //  fieldcage_displ_  (prev:  84.*mm),
  // FIELD SHAPING RINGS /////////////////////////
  ring_diam_   (235. * mm),  // from STEP-file // (prev: 229. * mm),
  ring_height_ ( 10. * mm),
  ring_thickn_ (  5. * mm),  //  ( 5. * mm) according to STEP ??? //  (prev:  6. * mm)
  ring_up_bt_gap_ (37.2 * mm),  //  from STEP-file
  // SUPPORT BARS ////////////////////////////////
  bar_length_  ((285. + 8.) * mm),  //  (prev:  expression) // (293 * mm)
  bar_width_   ( 19. * mm),  //  (prev:  40. * mm)
  bar_thickn_  ( 11. * mm),  //  (prev:  10. * mm),  (STEP file:  11. * mm)
  bar_addon1_length_ ( 7. * mm),  // (prev:  none)   (STEP file:   8. * mm) or ( 7. * mm)  ?
  bar_addon1_thickn_ (16. * mm),  // (prev:  ),
  bar_addon2_length_ (16.5 * mm),  // (prev:  50. * mm), (16.5 * mm) + ( 7. * mm) = tot: 23.5*mm
  bar_addon2_thickn_ ( 7. * mm),
  barBT_length_ ( 97.5 * mm),
  barBT_addonBT_length_ ( 7.5 * mm),
  barBT_addonBT_thickn_ ( 8.9 * mm),
  //////////////////////////////////////////////////
  // VESSEL
  // _vessel_length (600. * mm),
  // DEFAULT VALUES FOR SOME PARAMETERS///////////
  //_temperature(300.* kelvin),  // _temperature <<==>> 303 ; 4 places here

  max_step_size_(1.*mm),
  tpb_coating_(true),

    visibility_ (0),
    verbosity_(0),

    // EL field ON or OFF
    elfield_(0),
    el_table_index_(0),
    el_table_binning_(5.*mm)

  {

   /// Needed External variables
   vessel_length_ = vessel_length;

    // Define a new category
    new G4UnitDefinition("kilovolt/cm","kV/cm","Electric field", kilovolt/cm);
    new G4UnitDefinition("mm/sqrt(cm)","mm/sqrt(cm)","Diffusion", mm/sqrt(cm));

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/", "Control commands of geometry NextDemo.");
    msg_->DeclareProperty("field_cage_vis", visibility_, "Field Cage Visibility");
    msg_->DeclareProperty("field_cage_verbosity", verbosity_, "Field Cage verbosity");

  G4GenericMessenger::Command& max_step_size_cmd =
    msg_->DeclareProperty("max_step_size", max_step_size_, "Set maximum step size.");
  max_step_size_cmd.SetUnitCategory("Length");
  max_step_size_cmd.SetParameterName("max_step_size", true);
  max_step_size_cmd.SetRange("max_step_size>0");

    // Boolean-type properties (true or false)
   msg_->DeclareProperty("elfield", elfield_, "True if the EL field is on (full simulation), false if it's not (parametrized simulation).");

  msg_->DeclareProperty("tpb_coating", tpb_coating_,
			"True if the upper light tube is coated.");

  /// Temporary
  G4GenericMessenger::Command&  specific_vertex_X_cmd =
    msg_->DeclareProperty("specific_vertex_X", specific_vertex_X_,
   			  "If region is AD_HOC, x coord where particles are generated");
  specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
  specific_vertex_X_cmd.SetUnitCategory("Length");
  G4GenericMessenger::Command&  specific_vertex_Y_cmd =
    msg_->DeclareProperty("specific_vertex_Y", specific_vertex_Y_,
   			  "If region is AD_HOC, y coord where particles are generated");
  specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
  specific_vertex_Y_cmd.SetUnitCategory("Length");
  G4GenericMessenger::Command&  specific_vertex_Z_cmd =
    msg_->DeclareProperty("specific_vertex_Z", specific_vertex_Z_,
   			  "If region is AD_HOC, z coord where particles are generated");
  specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
  specific_vertex_Z_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command& gate_transparency_cmd =
    msg_->DeclareProperty("gate_transparency", gate_transparency_,
			  "Set the transparency of the gate EL mesh.");
  gate_transparency_cmd.SetParameterName("gate_transparency", false);
  gate_transparency_cmd.SetRange("gate_transparency>0 && gate_transparency<1");

  }

  // void NextDemoFieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)  // Ruty - like Next100
  void NextDemoFieldCage::SetLogicalVolume(G4LogicalVolume* mother_logic)          /////  Ruty - from New    /////
  {
    mother_logic_ = mother_logic;
    gas_ = mother_logic_->GetMaterial();
    pressure_ =    gas_->GetPressure();
    temperature_ = gas_->GetTemperature();
  }



  void NextDemoFieldCage::SetPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    mother_phys_ = mother_phys;
  }



  void NextDemoFieldCage::Construct()
  {
      // Store the position of the center of the fieldcage
      // RUTY from Next1EL.cc
      // VESSEL :: // _vessel_length (600. * mm);( _vessel_length/2. = 300*mm)
  fieldcage_position_.
    set(0., 0., (vessel_length_/2. -fieldcage_length_/2.-fieldcage_displ_));  // (0.,0.,11.45)*mm
    //set(0., 0., (300.*mm -_fieldcage_length/2.-_fieldcage_displ));  // (prev: 0.,0.,9.5)*mm

   G4double diel_thickn = .1*mm;

  // Position of the electrodes in the fieldcage
  anode_posz_   = fieldcage_length_/2.  - anode_length_/2.; // (prev: - elgap_ring_height_)
  gate_posz_    = anode_posz_ - anode_length_/2. - elgap_length_ ;   // It is the MESH posz not the GateHV posz // The mesh itself is in the EL_gap 0.1*mm //
  cathode_posz_ = gate_posz_  - GateHV_Mesh_Dist_ - active_length_;
  // World==Vessel Coordinates => EL_table_gate_posz = _gate_posz + _fieldcage_position.z
  el_table_z_ = gate_posz_ + fieldcage_position_.z();
   // 0.1 * mm is added to avoid very small negative numbers in drift lengths
  //SetELzCoord(el_table_z_ + 0.1 * mm); // Gate-Mesh Posz
 ////////////////////////
  //AnodezCoord_ = el_table_z_  + elgap_length_ + anode_length_/2.;  // To the Edge of EL_GAP or before the Anode, facing Sipm (+ anode_length_/2.) ??
 /////////////////////
   /// Ionielectrons are generated at a z = .5 mm inside the EL gap, not on border
  el_table_z_ = el_table_z_ + 0.5*mm;    // Next1EL.cc  SEE down here from: NextNewFieldCage.cc !!!
  //_el_table_z = _el_table_z + 0.2*mm;       // NextNewFieldCage.cc
   /*
   // For EL Table generation
   idx_table_ = 0;
  table_vertices_.clear();

   G4double z = vessel_length_/2. - fieldcage_displ_ - elgap_ring_height_ - elgap_length_;
   SetELzCoord(z);
   z = z +  .5*mm;
   CalculateELTableVertices(92.5*mm, 5.*mm, z);
   */

    // From::  NextNewFieldCage.cc
    G4double max_radius = floor(ltube_diam_/2./el_table_binning_)*el_table_binning_;
    //   _active_diam   (_ltube_diam),
    CalculateELTableVertices(max_radius, el_table_binning_, el_table_z_);
   // CalculateELTableVertices(92.5*mm, 5.*mm, z);    // Next1EL.cc

    // for (G4int i=0; i<_el_table_vertices.size(); ++i) {
    //   std::cout << i << ": "<< _el_table_vertices[i] << std::endl;
    // }

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

 // DefineMaterials()
  // ALUMINUM
  aluminum_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
  // PTFE (TEFLON)
  teflon_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  //TPB + +
  tpb_ = MaterialsList::TPB();
  tpb_->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
  // Quartz
  quartz_ =  MaterialsList::FusedSilica();
  quartz_->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());


  // ELECTROLUMINESCENCE GAP /////////////////////////////////////////

    // GATE AND ANODE (EL) RINGS ..... Modify due to STEP file ................

  G4Tubs* anode_ring_solid =
    new G4Tubs("ANODE_RING", anode_ring_diam_/2.,
	       (anode_ring_diam_/2. + anode_ring_thickn_),
	       anode_ring_length_/2., 0, twopi);

  G4LogicalVolume* anode_ring_logic =
    new G4LogicalVolume(anode_ring_solid, aluminum_, "ANODE_RING");

  G4double posz = fieldcage_length_/2. ; // STEP file (? - anode_length_), (prev: - elgap_ring_height_/2.)

  //new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()), anode_ring_logic,
  // 		    "ANODE_RING", mother_logic_, false, 0, true);

   // if (verbosity_) {
   //    G4cout << "Anode ring starts in " 
   //           << posz + fieldcage_position_.z() - anode_ring_length_/2. << G4endl
   //           << " and ends in " 
   //           << posz + fieldcage_position_.z() + anode_ring_length_/2. 
   //           << G4endl;
   // }
//////////////////////////////////////////////////////////////////////////////
  G4Tubs* elgap_ring_solid =
    new G4Tubs("EL_GAP_RING", elgap_ring_diam_/2.,
	       (elgap_ring_diam_/2. + elgap_ring_thickn_),
	       elgap_ring_height_/2., 0, twopi);

  G4LogicalVolume* elgap_ring_logic =
    new G4LogicalVolume(elgap_ring_solid, aluminum_, "EL_GAP_RING");

  // posz = _fieldcage_length/2. - _anode_ring_length/2. - 5.5 * mm - _elgap_ring_height/2.; 
  posz = posz - anode_ring_length_/2. - 5.5 * mm - elgap_ring_height_/2.;  // (1.1+4.4)*mm for distance, STEP file

  // new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()), elgap_ring_logic,
  //  		    "EL_GAP_RING", mother_logic_, false, 0, true);

  //  if (verbosity_) {
  //     G4cout << "Gate ring starts in " 
  //            << posz + fieldcage_position_.z() - elgap_ring_height_/2. << G4endl
  //            << " and ends in " 
  //            << posz + fieldcage_position_.z() + elgap_ring_height_/2. 
  //            << G4endl;
  //  }


  // if (visibility_) {
  // G4VisAttributes * visEL = new G4VisAttributes;
  // visEL->SetColor(0.2, 0.7, 0.3);
  // visEL->SetForceWireframe(true);
  // elgap_ring_logic->SetVisAttributes(visEL);
  // }
  // else {
  //   elgap_ring_logic->SetVisAttributes(G4VisAttributes::Invisible);
  // }

  // EL GAP ................................................

  G4Tubs* elgap_solid = new G4Tubs("EL_GAP", 0., elgap_ring_diam_/2.,
   				   elgap_length_/2., 0, twopi);

  G4LogicalVolume* elgap_logic =
    new G4LogicalVolume(elgap_solid, gas_, "EL_GAP");   // gxe_

  //posz = = fieldcage_length_/2. - anode_length_ - elgap_length_/2.;  // (STEP file:- anode_length_),  (prev: - elgap_ring_height_)
  G4double el_gap_zpos = GetELzCoord() - elgap_length_/2.;
  
  // new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()),
   new G4PVPlacement(0, G4ThreeVector(0.,0., el_gap_zpos),
                    elgap_logic, "EL_GAP", mother_logic_, false, 0, true);

  // Store the position of the EL GAP wrt the WORLD system of reference
  //elgap_position_.set(0.,0.,fieldcage_position_.z()+posz);

   // if (verbosity_) {
   //   G4cout << "  ****************************  " << G4endl;
   //   G4cout << "*** Positions of el_gap FC coordinates***" << G4endl;
   //   G4cout << "posz: " << posz << G4endl;
   //   G4cout << "Positions of el_gap world coordinate: " << posz + fieldcage_position_.z() << G4endl;
   //   G4cout << "elgap_position_() " << elgap_position_.z() << G4endl;
   //   G4cout << "EL GAP starts in " << posz + fieldcage_position_.z() - elgap_length_/2. << G4endl
   //          << " and ends in " <<  posz + fieldcage_position_.z() + elgap_length_/2. << G4endl;
   //   G4cout << "fieldcage_position_: " <<  fieldcage_position_ << G4endl;
   //   G4cout << "fieldcage_length_: " <<  fieldcage_length_ << G4endl;
   //   G4cout << "anode_position_ (wrld): " <<  anode_posz_ + fieldcage_position_.z() << G4endl;
   //   G4cout << "cathode_position_ (wrld): " <<  cathode_posz_ + fieldcage_position_.z() << G4endl;
   //   G4cout << "  ****************************  " << G4endl;
   // }
  //////////////////////////////////////////////////////////////////////////////////


  // EL GRIDS

  // G4double diel_thickn = .1*mm;


  G4Material* fgrid_gate = MaterialsList::FakeDielectric(gas_, "grid_mat");
  fgrid_gate->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(pressure_, temperature_,
									gate_transparency_, diel_thickn));

  G4Tubs* diel_grid =
    new G4Tubs("GRID_GATE", 0., elgap_ring_diam_/2., diel_thickn/2., 0, twopi);

  G4LogicalVolume* diel_grid_gate_logic =
    new G4LogicalVolume(diel_grid, fgrid_gate, "GRID_GATE");
  G4double pos1 = elgap_length_/2. - diel_thickn/2.;
  new G4PVPlacement(0, G4ThreeVector(0.,0.,pos1), diel_grid_gate_logic, "GRID_GATE",
		    elgap_logic, false, 0, true);


 //G4double anode_diam = _anode_quartz_diam;

    ///// ANODE //////

    G4Tubs* anode_quartz_solid =
      new G4Tubs("EL_QUARTZ_ANODE", 0., anode_diam_/2. , anode_length_/2., 0, twopi);

    G4LogicalVolume* anode_logic =
      new G4LogicalVolume(anode_quartz_solid, quartz_, "EL_QUARTZ_ANODE");

    //G4double pos_z_anode_ = anode_posz_ + fieldcage_position_.z();
    G4double pos_z_anode = GetELzCoord() - elgap_length_ - anode_length_/2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., pos_z_anode), anode_logic,
		      "EL_QUARTZ_ANODE", mother_logic_, false, 0, true);

   if (verbosity_) {
       G4cout << "Anode plate starts in " << pos_z_anode - anode_length_/2. << G4endl
              << " and ends in "          << pos_z_anode + anode_length_/2. << G4endl;
   }


  if (elfield_) {
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    G4double global_el_gap_zpos = el_gap_zpos - GetELzCoord();
    el_field->SetCathodePosition(global_el_gap_zpos + elgap_length_/2.);
    el_field->SetAnodePosition(global_el_gap_zpos - elgap_length_/2.);
    el_field->SetDriftVelocity(2.5*mm/microsecond);
    el_field->SetTransverseDiffusion(1.*mm/sqrt(cm));
    el_field->SetLongitudinalDiffusion(.5*mm/sqrt(cm));
    XenonGasProperties xgp(pressure_, temperature_);
    // el_field->SetLightYield(xgp.ELLightYield(24.8571*kilovolt/cm));//value for E that gives Y=1160 photons per ie- in normal conditions
    el_field->SetLightYield(xgp.ELLightYield(23.2857*kilovolt/cm));
    G4Region* el_region = new G4Region("EL_REGION");
    el_region->SetUserInformation(el_field);
    el_region->AddRootLogicalVolume(elgap_logic);
  }

  // ACTIVE VOLUME ///////////////////////////////////////////////////

  // Position of z planes
  G4double zplane[2] = {-active_length_/2., active_length_/2.};
  // Inner radius
  G4double rinner[2] = {0., 0.};
  // Outer radius
  G4double router[2] = {active_diam_/2., active_diam_/2.};

  G4Polyhedra* active_solid =
    // new G4Polyhedra("ACTIVE", 0., twopi, 6, 2, zplane, rinner, router);
    new G4Polyhedra("ACTIVE", 0., twopi, 10, 2, zplane, rinner, router);

  G4LogicalVolume* active_logic =
    new G4LogicalVolume(active_solid, gas_, "ACTIVE");

  //   posz = fieldcage_length_/2.  - anode_length_ - elgap_length_ - GateHV_Mesh_Dist_ - active_length_/2.;
  G4double active_zpos = GetELzCoord() + active_length_/2.;

    //new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()),
    new G4PVPlacement(0, G4ThreeVector(0.,0., active_zpos),
                    active_logic, "ACTIVE", mother_logic_, false, 0, true);

  // Store the position of the active volume with respect to the
  // WORLD system of reference
  //active_position_.set(0.,0.,fieldcage_position_.z()+posz);

  //_hexrnd = new HexagonPointSampler(_active_diam/2., _active_length, 0.,
				  //  active_position_);

  // Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

  // Set the volume as an ionization sensitive detector
  // G4String det_name = "/DEMO/ACTIVE";
  // IonizationSD* ionisd = new IonizationSD(det_name);
  IonizationSD* ionisd = new IonizationSD("/DEMO/ACTIVE");
  active_logic->SetSensitiveDetector(ionisd);
  G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

  //Define a drift field for this volume
  UniformElectricDriftField* field = new UniformElectricDriftField();
  G4double global_active_zpos = active_zpos - GetELzCoord();
  field->SetCathodePosition(global_active_zpos + active_length_/2.);
  field->SetAnodePosition(global_active_zpos - active_length_/2.);
  field->SetDriftVelocity(1.*mm/microsecond);
  field->SetTransverseDiffusion(1.*mm/sqrt(cm));
  field->SetLongitudinalDiffusion(.3*mm/sqrt(cm));

  G4Region* drift_region = new G4Region("DRIFT");
  drift_region->SetUserInformation(field);
  drift_region->AddRootLogicalVolume(active_logic);

  active_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // CATHODE .....................................

  diel_thickn = 1. * mm;
  G4double transparency = 0.98;

  G4Material* fcathode = MaterialsList::FakeDielectric(gas_, "cathode_mat");
  fcathode->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(pressure_, temperature_,
  									transparency, diel_thickn));

  G4Tubs* diel_cathd =
    new G4Tubs("CATHODE", 0., cathode_ring_diam_/2., diel_thickn/2., 0, twopi);  // elgap_ring_diam_/2

  G4LogicalVolume* diel_cathd_logic =
    new G4LogicalVolume(diel_cathd, fcathode, "CATHODE");

  //posz = cathode_posz_ - diel_thickn/2.;
  G4double cathode_zpos = GetELzCoord() + active_length_ + diel_thickn/2.;

  //new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()),
  new G4PVPlacement(0, G4ThreeVector(0.,0., cathode_zpos),
                    diel_cathd_logic, "CATHODE", mother_logic_, false, 0, true);


  // LIGHT TUBE //////////////////////////////////////////////////////
  // Hexagonal prism of teflon used as light tube. It transports the
  // EL light from anode to cathode improving the light collection
  // efficiency of the energy plane.

  // UPPER PART ............................................

  zplane[0] = -ltube_up_length_/2.; zplane[1] = ltube_up_length_/2.;
  rinner[0] = ltube_diam_/2.; rinner[1] = rinner[0];
  router[0] = ltube_diam_/2. + ltube_thickn_; router[1] = router[0];

  G4Polyhedra* ltube_up_solid =
    // new G4Polyhedra("LIGHT_TUBE_UP", 0., twopi, 6, 2, zplane, rinner, router);
    new G4Polyhedra("LIGHT_TUBE_UP", 0., twopi, 10, 2, zplane, rinner, router);

  G4LogicalVolume* ltube_up_logic =
    new G4LogicalVolume(ltube_up_solid, teflon_, "LIGHT_TUBE_UP");

  //posz = (gate_posz_ - GateHV_Mesh_Dist_ + cathode_posz_) / 2.;  // STEP file: ( - GateHV_Mesh_Dist_)
  G4double lt_up_zpos = (GetELzCoord() + GateHV_Mesh_Dist_ + cathode_zpos)/2.;

  //new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()),
  new G4PVPlacement(0, G4ThreeVector(0.,0., lt_up_zpos),
                    ltube_up_logic, "LIGHT_TUBE_UP", mother_logic_, false, 0, true);

  // if (verbosity_) {
  //       G4cout << "  ****************************  " << G4endl;
  //       G4cout << "*** Positions of internal upper light tubs ***" << G4endl;
  //       G4cout << "posz: " << posz << G4endl;
  //       G4cout << "fieldcage_position_.z() : "  <<  fieldcage_position_.z() << G4endl;
  //       G4cout << "LIGHT_TUBE_UP.Z: " << posz + fieldcage_position_.z() << G4endl;
  //       G4cout << "LIGHT_TUBE_UP.Z edge: " << posz + fieldcage_position_.z() + ltube_up_length_/2. << G4endl;
  //       G4cout << "  ****************************  " << G4endl;
  // }

  // TPB coating
  //if (_tpb_coating) {
    G4double rinner_tpb[2];
    G4double router_tpb[2];
    rinner_tpb[0] = ltube_diam_/2.; rinner_tpb[1] = rinner_tpb[0];
    router_tpb[0] = ltube_diam_/2. + tpb_thickn_; router_tpb[1] = router_tpb[0];

    G4Polyhedra* ltube_tpb_solid =
      //  new G4Polyhedra("LIGHT_TUBE_TPB", 0., twopi, 6, 2, zplane,
      new G4Polyhedra("LIGHT_TUBE_TPB", 0., twopi, 10, 2, zplane,
		      rinner_tpb, router_tpb);
    G4LogicalVolume* ltube_tpb_logic =
      new G4LogicalVolume(ltube_tpb_solid, tpb_, "LIGHT_TUBE_TPB");

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

  zplane[0] = -ltube_bt_length_/2.; zplane[1] = ltube_bt_length_/2.;

  G4Polyhedra* ltube_bt_solid =
    // new G4Polyhedra("LIGHT_TUBE_BOTTOM", 0., twopi, 6, 2, zplane, rinner, router);
    new G4Polyhedra("LIGHT_TUBE_BOTTOM", 0., twopi, 10, 2, zplane, rinner, router);

  G4LogicalVolume* ltube_bt_logic =
    new G4LogicalVolume(ltube_bt_solid, teflon_, "LIGHT_TUBE_BOTTOM");

  // posz = (gate_posz_ - GateHV_Mesh_Dist_ + cathode_posz_) / 2.     // STEP file: ( - GateHV_Mesh_Dist_)
  //        -  ltube_up_length_/2.  - ltube_gap_  - ltube_bt_length_/2.;
  G4double lt_bottom_zpos = lt_up_zpos + ltube_up_length_/2.  + ltube_gap_  + ltube_bt_length_/2.;

  //new G4PVPlacement(0, G4ThreeVector(0.,0.,posz + fieldcage_position_.z()),
  new G4PVPlacement(0, G4ThreeVector(0.,0., lt_bottom_zpos),
                    ltube_bt_logic, "LIGHT_TUBE_BOTTOM", mother_logic_, false, 0, true);

///////////////////////////////////////////////////////////////////////////////////
   // if (verbosity_) {
   //         G4cout << "  ****************************  " << G4endl;
   //         G4cout << "*** Positions of internal light tubs ***" << G4endl;
   //         G4cout << "posz: " << posz << G4endl;
   //         G4cout << "LIGHT_TUBE_BOTTOM.Z: " << posz + fieldcage_position_.z() << G4endl;
   //         G4cout << "fieldcage_position_: " <<  fieldcage_position_ << G4endl;
   //         G4cout << "ACTIVE: " << active_position_ << G4endl;
   //         G4cout << "  ****************************  " << G4endl;
   // }
//////////////////////////////////////////////////////////////////////////////////

  // SETTING VISIBILITIES   //////////
  if (visibility_) {
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

  G4Tubs* ring_solid = new G4Tubs("FIELD_RING", ring_diam_/2.,
   				  (ring_diam_/2.+ring_thickn_),
  				  ring_height_/2., 0, twopi);

  G4LogicalVolume* ring_logic =
    new G4LogicalVolume(ring_solid, aluminum_, "FIELD_RING");


  // // DRIFT REGION ................................
   if (verbosity_) {
       G4cout << "  ***** RINGS DRIFT REGION* ****  " << G4endl;
       G4cout << "*** Positions  Z: ***" << G4endl;
   }

  G4int num_rings = 19;

  posz = fieldcage_length_/2. - anode_length_ - elgap_length_
        - GateHV_Mesh_Dist_ *2. - ring_height_/2.;  //  (- 10.*mm + 1.2 *mm) ?

  // RUTY ADD::    " _fieldcage_position.z() "  to  posZ

  for (G4int i=0; i<num_rings; i++) {
      if (verbosity_) {
         G4cout << "posz (wrld): " <<  i  << " :  " <<  posz  + fieldcage_position_.z() << G4endl;
      }
    // new G4PVPlacement(0, G4ThreeVector(0., 0., posz + fieldcage_position_.z()), ring_logic,
    //             "FIELD_RING", mother_logic_, false, i, true);

    //  prev:  posz = posz - _ring_height - 5.1 * mm;
    posz = posz - ring_height_ - 5.0 * mm;

  }

  // // BUFFER ......................................
   if (verbosity_) {
       G4cout << "  ***** RINGS BUFFER REGION* ****  " << G4endl;
       G4cout << "*** Positions  Z: ***" << G4endl;
   }

  posz = posz + ring_height_ + 5.0 * mm - ring_up_bt_gap_;

  for (G4int i=19; i<23; i++) {
     if (verbosity_) {

        G4cout << "posz (wrld): "  <<  i  << " :  " << posz  + fieldcage_position_.z() << G4endl;
     }

    // new G4PVPlacement(0, G4ThreeVector(0., 0., posz  + fieldcage_position_.z()), ring_logic,
    //             "FIELD_RING", mother_logic_, false, i, true);

    posz = posz - ring_height_ - 10. * mm;

  }

  // SETTING VISIBILITIES   //////////
  //_visibility = 1;
  if (visibility_) {
    G4VisAttributes yellow_col =nexus::Yellow();
    yellow_col.SetForceSolid(true);
    ring_logic->SetVisAttributes(yellow_col);
   }
   else {
    ring_logic->SetVisAttributes(G4VisAttributes::Invisible);
   }
  //_visibility = 0;



  // SUPPORT BARS - DRIFT ////////////////////////////////////////////////////

  G4Box* bar_base = new G4Box("SUPPORT_BAR", bar_thickn_/2.,
			 bar_width_/2., bar_length_/2.);

  G4Box* addon1 = new G4Box("SUPPORT_BAR", bar_addon1_thickn_/2.,
			   bar_width_/2., bar_addon1_length_/2.);

  G4Box* addon2 = new G4Box("SUPPORT_BAR", bar_addon2_thickn_/2.,
			   bar_width_/2., bar_addon2_length_/2.);

  //  UNIONS for support bar (pillar) and add_on

  G4UnionSolid* bar_solid =
    new G4UnionSolid("SUPPORT_BAR", bar_base, addon1, 0,
		     G4ThreeVector((bar_thickn_ + bar_addon1_thickn_)/2., 0.,
                                   -(bar_length_ - bar_addon1_length_)/2.)); // + or -

  bar_solid =
    new G4UnionSolid("SUPPORT_BAR", bar_solid, addon2, 0,
                  G4ThreeVector(bar_thickn_/2. + bar_addon1_thickn_ - bar_addon2_thickn_/2., 0,
                                -(bar_length_ + bar_addon2_length_)/2.));


  G4LogicalVolume* bar_logic =
    new G4LogicalVolume(bar_solid, MaterialsList::PEEK(), "SUPPORT_BAR");
    //new G4LogicalVolume(bar_base, MaterialsList::PEEK(), "SUPPORT_BAR");

  G4double pos_rad = ltube_diam_/2. + ltube_thickn_ + bar_thickn_/2.;
  posz = fieldcage_length_/2. - anode_ring_length_/2. - 5.6 * mm - elgap_ring_height_ // - ;// (5.5*mm OR elgap_length_/2.)
  //posz = _fieldcage_length/2. - 2.*_elgap_ring_height - _elgap_length // prev.
       - bar_length_/2.;

   if (verbosity_) {
       G4cout << "  ******  SUPPORT_BAR  ******  " << G4endl;
       G4cout << "SUPPORT_BAR.Z: " << posz + fieldcage_position_.z() << G4endl;
       G4cout << "SUPPORT_BAR.Z up: " << posz + fieldcage_position_.z() + bar_length_/2. << G4endl;
       G4cout << "SUPPORT_BAR.Z bt: " << posz + fieldcage_position_.z() - bar_length_/2. << G4endl;
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

    // new G4PVPlacement(G4Transform3D(rotbar, G4ThreeVector(xx, yy, zz + fieldcage_position_.z())),
    //     	      bar_logic, "SUPPORT_BAR", mother_logic_, false, i, true);
  }
   /*G4VisAttributes * vis3 = new G4VisAttributes;
   vis3->SetColor(0.5, 0.5, .5);
   vis3->SetForceSolid(true);
   bar_logic->SetVisAttributes(vis3);*/

   // if (visibility_) {
   // G4VisAttributes dirty_white_col =nexus::DirtyWhite();
   // dirty_white_col.SetForceSolid(true);
   // bar_logic->SetVisAttributes(dirty_white_col);
   // }
   // else {
   //   bar_logic->SetVisAttributes(G4VisAttributes::Invisible);
   // }

  // SUPPORT BARS - BUFFER //////////////////////////////////////////////

  G4Box* barBT_base = new G4Box("SUPPORT_BAR_BT", bar_thickn_/2.,
			 bar_width_/2., barBT_length_/2.);

  G4Box* addonBT = new G4Box("SUPPORT_BAR_BT", barBT_addonBT_thickn_/2.,
			   bar_width_/2., barBT_addonBT_length_/2.);

  G4UnionSolid* barBT_solid =
    new G4UnionSolid("SUPPORT_BAR_BT", barBT_base, addonBT, 0,
		     G4ThreeVector((bar_thickn_ + barBT_addonBT_thickn_)/2., 0.,
                                   (barBT_length_ - barBT_addonBT_length_)/2.));

  G4LogicalVolume* barBT_logic =
    new G4LogicalVolume(barBT_solid, MaterialsList::PEEK(), "SUPPORT_BAR_BT");

  posz = (gate_posz_ - GateHV_Mesh_Dist_ + cathode_posz_) / 2.     // STEP file: ( - GateHV_Mesh_Dist_)
         -  ltube_up_length_/2.  - ltube_gap_  - ltube_bt_length_/2. + 4.6 * mm; // poszBT + 4.35 * mm

   if (verbosity_) {
	  G4cout << "  ******  SUPPORT_BAR BT  ******  " << G4endl;
	  G4cout << "SUPPORT_BAR_BT.Z: " << posz + fieldcage_position_.z() << G4endl;
	  G4cout << "SUPPORT_BAR_BT.Z up: " << posz + fieldcage_position_.z() + barBT_length_/2. << G4endl;
	  G4cout << "SUPPORT_BAR_BT.Z bt: " << posz + fieldcage_position_.z() - barBT_length_/2. << G4endl;
	  G4cout << "  **************   " << G4endl;
	  G4cout << " gate_transparency_: " <<  gate_transparency_  << G4endl;
	  G4cout << "  max_step_size_  : " <<    max_step_size_     << G4endl;
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

    // new G4PVPlacement(G4Transform3D(rotbar, G4ThreeVector(xx, yy, zz + fieldcage_position_.z())),
    //     	      barBT_logic, "SUPPORT_BAR_BT", mother_logic_, false, i, true);
  }


  if (visibility_) {
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
  if (tpb_coating_) {
    //ltubeup_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_with_TPB());
  //} else {
   // ltubeup_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

    G4OpticalSurface* gas_tpb_teflon_surf =
      new G4OpticalSurface("GAS_TPB_TEFLON_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);

    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", ltube_tpb_phys, mother_phys_,
                               gas_tpb_teflon_surf);
    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", mother_phys_, ltube_tpb_phys,
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
				    //active_position_);

     decrnd_gen_ = new DecagonPointSampler(active_diam_/2., active_length_, 0., active_position_);


  }



  NextDemoFieldCage::~NextDemoFieldCage()
  {
    //delete _hexrnd;
    delete decrnd_gen_;
  }



  G4ThreeVector NextDemoFieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    
     if (region == "ACTIVE") {
      //vertex = _hexrnd_gen->GenerateVertex(INSIDE);
      //vertex = _hexrnd_gen->GenerateVertex(PLANE);
      vertex = decrnd_gen_->GenerateVertex(INSIDE10);
    }
    else if (region == "AD_HOC") {
      vertex = G4ThreeVector(specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_);
    }
    else if (region == "EL_TABLE") {

      unsigned int i = el_table_point_id_ + el_table_index_;

      if (i == (el_table_vertices_.size()-1)) {
        G4Exception("[NextDemoFieldcage]", "GenerateVertex()",
		    RunMustBeAborted, "Reached last event in EL lookup table.");
      }

      try {
        vertex = el_table_vertices_.at(i);
        el_table_index_++;
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
          el_table_vertices_.push_back(xyz);
      }
    }
  }



  G4ThreeVector NextDemoFieldCage::GetPosition() const
  {
    return G4ThreeVector (0., 0., fieldcage_position_.z());
  }

  G4double NextDemoFieldCage::GetAnodezCoord() const
  {
    return AnodezCoord_;
  }


}
