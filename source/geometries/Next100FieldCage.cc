// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <paola.ferrario@dipc.org>, <jmunoz@ific.uv.es>
//  Created: 1 Mar 2012
//
//  Copyright (c) 2012-2020 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100FieldCage.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"
#include "XenonGasProperties.h"

#include <G4GenericMessenger.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4UnitsTable.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  Next100FieldCage::Next100FieldCage():
    BaseGeometry(),
    // Field cage dimensions
    _active_diam (984 * mm), // distance between the centers of two opposite panels
    _active_length (1159.6 * mm), // distance between gate and cathode
    _drift_transv_diff(1. * mm/sqrt(cm)),
    _drift_long_diff(.3 * mm/sqrt(cm)),
    _max_step_size(1. * mm),
    _buffer_length (282. * mm), // distance between cathode and sapphire window surfaces
    _grid_thickn (.1 * mm),
    _teflon_drift_length (1159.6 * mm), // to check with final design
    _teflon_buffer_length (282. * mm), // to check with final design
    _teflon_thickn (5 * mm),
    _npanels (18),
    _tpb_thickn (1 * micrometer),
    _el_gap_length (1. * cm),
    _ELtransv_diff (0. * mm/sqrt(cm)),
    _ELlong_diff (0. * mm/sqrt(cm)),
    _ELelectric_field (34.5*kilovolt/cm),
    _el_grid_transparency (.88), // to check
    _elfield(0),
    _visibility (1),
    _verbosity (0)
  {

    /// Calculate derived positions
    _active_zpos = _active_length/2.;
    _el_gap_zpos = _active_zpos - _active_length/2. - _el_gap_length/2.;

    // Define new categories
    new G4UnitDefinition("kilovolt/cm","kV/cm","Electric field", kilovolt/cm);
    new G4UnitDefinition("mm/sqrt(cm)","mm/sqrt(cm)","Diffusion", mm/sqrt(cm));

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");
    _msg->DeclareProperty("field_cage_vis", _visibility, "Field Cage Visibility");
    _msg->DeclareProperty("field_cage_verbosity", _verbosity, "Field Cage Verbosity");

    G4GenericMessenger::Command& drift_transv_diff_cmd =
      _msg->DeclareProperty("drift_transv_diff", _drift_transv_diff,
			    "Tranvsersal diffusion in the drift region");
    drift_transv_diff_cmd.SetParameterName("drift_transv_diff", true);
    drift_transv_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command& drift_long_diff_cmd =
      _msg->DeclareProperty("drift_long_diff", _drift_long_diff,
			    "Longitudinal diffusion in the drift region");
    drift_long_diff_cmd.SetParameterName("drift_long_diff", true);
    drift_long_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command&  ELtransv_diff_cmd =
      _msg->DeclareProperty("ELtransv_diff", _ELtransv_diff,
			    "Tranvsersal diffusion in the EL region");
    ELtransv_diff_cmd.SetParameterName("ELtransv_diff", true);
    ELtransv_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command&  ELlong_diff_cmd =
      _msg->DeclareProperty("ELlong_diff", _ELlong_diff,
			    "Longitudinal diffusion in the EL region");
    ELlong_diff_cmd.SetParameterName("ELlong_diff", true);
    ELlong_diff_cmd.SetUnitCategory("Diffusion");


    G4GenericMessenger::Command& El_field_cmd =
      _msg->DeclareProperty("EL_field", _ELelectric_field,
			    "Electric field in the EL region");
    El_field_cmd.SetParameterName("EL_field", true);
    El_field_cmd.SetUnitCategory("Electric field");


  }

  void Next100FieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }


  void Next100FieldCage::Construct()
  {
    // Define materials to be used
    DefineMaterials();
    //BuildCathodeGrid();
    BuildELRegion();
    BuildActive();
    BuildBuffer();
    BuildFieldCage();

    // Anode mesh
    //  BuildAnodeGrid();
    // Proper field cage and light tube
    //  BuildLightTube();
  }


  void  Next100FieldCage::DefineMaterials()
  {
    _gas         = _mother_logic->GetMaterial();
    _pressure    = _gas->GetPressure();
    _temperature = _gas->GetTemperature();

    // High density polyethylene for the field cage
    _hdpe = MaterialsList::HDPE();

    // Copper for field rings
    _copper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

    // Teflon for the light tube
    _teflon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

    // TPB coating
    _tpb = MaterialsList::TPB();
    _tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());

    //ITO coating
    //  _ito = MaterialsList::ITO();
    // _ito->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeFusedSilica(_ito_transparency, _ito_thickness));
  }


    void Next100FieldCage::BuildActive()
  {
    /// ACTIVE ///

    // Position of z planes
    G4double zplane[2] = {-_active_length/2., _active_length/2.};
    // Inner radius
    G4double rinner[2] = {0., 0.};
    // Outer radius
    G4double router[2] = {_active_diam/2., _active_diam/2.};

    G4Polyhedra* active_solid =
      new G4Polyhedra("ACTIVE", 0., twopi, 18, 2, zplane, rinner, router);

    G4LogicalVolume* active_logic = new G4LogicalVolume(active_solid, _gas, "ACTIVE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., _active_zpos), active_logic,
		      "ACTIVE", _mother_logic, false, 0, false);

    // Limit the step size in this volume for better tracking precision
    active_logic->SetUserLimits(new G4UserLimits(_max_step_size));

    // Set the volume as an ionization sensitive detector
    IonizationSD* ionisd = new IonizationSD("/NEXT100/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    //Define a drift field for this volume
    UniformElectricDriftField* field = new UniformElectricDriftField();
    field->SetCathodePosition(_active_zpos + _active_length/2.);
    field->SetAnodePosition(_active_zpos - _active_length/2.);
    field->SetDriftVelocity(1. * mm/microsecond);
    field->SetTransverseDiffusion(_drift_transv_diff);
    field->SetLongitudinalDiffusion(_drift_long_diff);
    G4Region* drift_region = new G4Region("DRIFT");
    drift_region->SetUserInformation(field);
    drift_region->AddRootLogicalVolume(active_logic);


    // Visibilities
    active_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // G4VisAttributes active_col = nexus::Red();
    // active_col.SetForceSolid(true);
    // active_logic->SetVisAttributes(active_col);

    // Vertex generator
    //  _active_gen = new HexagonPointSampler(_active_diam/2., _active_length, 0.,
    //					  G4ThreeVector(0., 0., _active_zpos));
  }


   void Next100FieldCage::BuildELRegion()
  {
    /// EL GAP
    G4double el_gap_diam = _active_diam; // TO CHECK

    G4Tubs* el_gap_solid =
      new G4Tubs("EL_GAP", 0., el_gap_diam/2., _el_gap_length/2., 0, twopi);

    G4LogicalVolume* el_gap_logic =
      new G4LogicalVolume(el_gap_solid, _gas, "EL_GAP");

    new G4PVPlacement(0, G4ThreeVector(0., 0., _el_gap_zpos), el_gap_logic,
		      "EL_GAP", _mother_logic, false, 0);

    if (_verbosity) {
      G4cout << "EL GAP starts in " << _el_gap_zpos - _el_gap_length/2.
	     << " and ends in " << _el_gap_zpos + _el_gap_length/2. << G4endl;
    }

    if (_elfield) {
      // Define EL electric field
      UniformElectricDriftField* el_field = new UniformElectricDriftField();
      el_field->SetCathodePosition(_el_gap_zpos + _el_gap_length/2.);
      el_field->SetAnodePosition  (_el_gap_zpos - _el_gap_length/2.);
      el_field->SetDriftVelocity(2.5 * mm/microsecond);
      el_field->SetTransverseDiffusion(_ELtransv_diff);
      el_field->SetLongitudinalDiffusion(_ELlong_diff);
      XenonGasProperties xgp(_pressure, _temperature);
      el_field->SetLightYield(xgp.ELLightYield(_ELelectric_field));
      G4Region* el_region = new G4Region("EL_REGION");
      el_region->SetUserInformation(el_field);
      el_region->AddRootLogicalVolume(el_gap_logic);
    }

    /// EL GRIDS
    G4Material* fgrid_mat = MaterialsList::FakeDielectric(_gas, "el_grid_mat");
    fgrid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature, _el_grid_transparency, _grid_thickn));

    // Dimensions & position: the grids are simulated inside the EL gap.
    // Their thickness is symbolic.
    G4double grid_diam = _active_diam; // TO CHECK

    G4double posz1 = _el_gap_length/2. - _grid_thickn/2.;
    G4double posz2 = -_el_gap_length/2. + _grid_thickn/2.;

    // _el_grid_ref_z = posz1;

    G4Tubs* diel_grid_solid =
      new G4Tubs("EL_GRID", 0., grid_diam/2., _grid_thickn/2., 0, twopi);

    G4LogicalVolume* diel_grid_logic =
      new G4LogicalVolume(diel_grid_solid, fgrid_mat, "EL_GRID");

    new G4PVPlacement(0, G4ThreeVector(0., 0., posz1), diel_grid_logic, "EL_GRID_1",
		      el_gap_logic, false, 0, false);
    new G4PVPlacement(0, G4ThreeVector(0., 0., posz2), diel_grid_logic, "EL_GRID_2",
     		      el_gap_logic, false, 1, false);

    /// Visibilities
    if (_visibility) {
      G4VisAttributes light_blue = nexus::LightBlue();
      light_blue.SetForceSolid(true);
      el_gap_logic->SetVisAttributes(light_blue);
    } else {
      el_gap_logic->SetVisAttributes(G4VisAttributes::Invisible);
      diel_grid_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }


  void Next100FieldCage::BuildBuffer()
  {

    G4double buffer_zpos =
      _active_zpos + _active_length/2. + _grid_thickn + _buffer_length/2.;

    // Position of z planes
    G4double zplane[2] = {-_buffer_length/2., _buffer_length/2.};
    // Inner radius
    G4double rinner[2] = {0., 0.};
    // Outer radius
    G4double router[2] = {_active_diam/2., _active_diam/2.};

    G4Polyhedra* buffer_solid =
      new G4Polyhedra("BUFFER", 0., twopi, 18, 2, zplane, rinner, router);

    if (_verbosity) {
      G4cout << "Buffer (gas) starts in " << buffer_zpos - _buffer_length/2.
	     << " and ends in "
	     << buffer_zpos + _buffer_length/2. << G4endl;
      }

    G4LogicalVolume* buffer_logic = new G4LogicalVolume(buffer_solid, _gas, "BUFFER");
    new G4PVPlacement(0, G4ThreeVector(0., 0., buffer_zpos), buffer_logic,
		      "BUFFER", _mother_logic, false, 0, false);

     // Set the volume as an ionization sensitive detector
    IonizationSD* buffsd = new IonizationSD("/NEXT100/BUFFER");
    buffsd->IncludeInTotalEnergyDeposit(false);
    buffer_logic->SetSensitiveDetector(buffsd);
    G4SDManager::GetSDMpointer()->AddNewDetector(buffsd);

    //  _buffer_gen =
    //   new CylinderPointSampler(0., _buffer_length, _active_diam/2.,
    // 			       0., G4ThreeVector (0., 0., buffer_posz));

    // // VERTEX GENERATOR FOR ALL XENON
    // G4double xenon_posz = (_buffer_length * buffer_posz +
    // 			   _active_length * _active_posz +
    // 			   _grid_thickn * _cathode_pos_z +
    // 			   _grid_thickn * _el_grid_ref_z +
    // 			   _el_gap_length * _el_gap_posz) / (_buffer_length +
    // 							     _active_length +
    // 							     2 * _grid_thickn +
    // 							     _el_gap_length);
    // G4double xenon_len = _buffer_length + _active_length + _grid_thickn;
    // _xenon_gen =
    //   new CylinderPointSampler(0., xenon_len, _active_diam/2.,
    // 			       0., G4ThreeVector (0., 0., xenon_posz));


    buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // G4VisAttributes active_col = nexus::Yellow();
    // active_col.SetForceSolid(true);
    // buffer_logic->SetVisAttributes(active_col);

  }


  void Next100FieldCage::BuildFieldCage()
  {

    /// DRIFT PART ///
    G4double teflon_drift_zpos = _active_zpos; //  to check with final design

    // Position of z planes
    G4double zplane[2] = {-_teflon_drift_length/2., _teflon_drift_length/2.};
    // Inner radius
    G4double rinner[2] = {_active_diam/2., _active_diam/2.};
    // Outer radius
    G4double router[2] =
      {(_active_diam + 2.*_teflon_thickn)/2., (_active_diam + 2.*_teflon_thickn)/2.};

    G4Polyhedra* teflon_drift_solid =
      new G4Polyhedra("LIGHT_TUBE_DRIFT", 0., twopi, 18, 2, zplane, rinner, router);

    G4LogicalVolume* teflon_drift_logic =
      new G4LogicalVolume(teflon_drift_solid, _teflon, "LIGHT_TUBE_DRIFT");

    new G4PVPlacement(0, G4ThreeVector(0., 0., teflon_drift_zpos), teflon_drift_logic,
		      "LIGHT_TUBE_DRIFT", _mother_logic, false, 0, false);


    /// TPB on teflon surface
    G4double router_tpb[2] =
      {(_active_diam + 2.*_tpb_thickn)/2., (_active_diam + 2.*_tpb_thickn)/2.};

    G4Polyhedra* tpb_solid =
      new  G4Polyhedra("DRIFT_TPB", 0., twopi, 18, 2, zplane, rinner, router_tpb);
    G4LogicalVolume* tpb_logic =
      new G4LogicalVolume(tpb_solid, _tpb, "DRIFT_TPB");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), tpb_logic,
  		      "DRIFT_TPB", teflon_drift_logic, false, 0, false);

    /// BUFFER PART ///

    G4double teflon_buffer_zpos =
      _active_zpos + _active_length/2. + _grid_thickn + _buffer_length/2; // TO CHECK

    G4double zplane_buff[2] = {-_teflon_buffer_length/2., _teflon_buffer_length/2.};
    G4double router_buff[2] =
      {(_active_diam + 2.*_teflon_thickn)/2., (_active_diam + 2.*_teflon_thickn)/2.};

    G4Polyhedra* teflon_buffer_solid =
      new G4Polyhedra("LIGHT_TUBE_BUFFER", 0., twopi, 18, 2,
		      zplane_buff, rinner, router_buff);

    G4LogicalVolume* teflon_buffer_logic =
      new G4LogicalVolume(teflon_buffer_solid, _teflon, "LIGHT_TUBE_BUFFER");

    new G4PVPlacement(0, G4ThreeVector(0., 0., teflon_buffer_zpos), teflon_buffer_logic,
		      "LIGHT_TUBE_BUFFER", _mother_logic, false, 0, false);

    /// TPB on teflon surface
    G4double router_tpb_buff[2] =
      {(_active_diam + 2.*_tpb_thickn)/2., (_active_diam + 2.*_tpb_thickn)/2.};

    G4Polyhedra* tpb_buffer_solid =
      new  G4Polyhedra("BUFFER_TPB", 0., twopi, 18, 2,
		       zplane_buff, rinner, router_tpb_buff);
    G4LogicalVolume* tpb_buffer_logic =
      new G4LogicalVolume(tpb_buffer_solid, _tpb, "BUFFER_TPB");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), tpb_buffer_logic,
  		      "BUFFER_TPB", teflon_buffer_logic, false, 0, false);


    /// Optical surface on teflon ///
    G4OpticalSurface* refl_Surf =
      new G4OpticalSurface("refl_Surf", unified, ground, dielectric_metal, .01);
    refl_Surf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_with_TPB());
    new G4LogicalSkinSurface("refl_teflon_surf", teflon_drift_logic, refl_Surf);
    new G4LogicalSkinSurface("refl_teflon_surf", teflon_buffer_logic, refl_Surf);

    /// Optical surface on TPB to model roughness ///
    G4OpticalSurface* owls_Surf =
      new G4OpticalSurface("oWLS_Surf", glisur, ground, dielectric_metal, .01);
    new G4LogicalSkinSurface("oWLS_teflon_surf", tpb_logic, owls_Surf);
    new G4LogicalSkinSurface("oWLS_teflon_surf", tpb_buffer_logic, owls_Surf);



    // SETTING VISIBILITIES   //////////

    if (_visibility) {
      G4VisAttributes light_blue = nexus::LightGreen();
      G4VisAttributes blue = nexus::DarkGreen();
      blue.SetForceSolid(true);
      light_blue.SetForceSolid(true);
      teflon_drift_logic->SetVisAttributes(light_blue);
      teflon_buffer_logic->SetVisAttributes(blue);
    }
    else {
      teflon_drift_logic->SetVisAttributes(G4VisAttributes::Invisible);
      teflon_buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // // VERTEX GENERATORS   //////////
    // _body_gen  = new CylinderPointSampler(_tube_diam/2. + _tpb_thickn, _tube_length, _tube_thickn - _tpb_thickn,
    //                                       0., G4ThreeVector (0., 0., _tube_zpos));
  }



  Next100FieldCage::~Next100FieldCage()
  {
    //delete _body_gen;
  }



  G4ThreeVector Next100FieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Vertex in the plastic cylinder
    // if (region == "FIELD_CAGE") {
    //   vertex = _body_gen->GenerateVertex("BODY_VOL");
    // }
    // else {
    //   G4Exception("[Next100FieldCage]", "GenerateVertex()", FatalException,
    // 		  "Unknown vertex generation region!");
    // }

    return vertex;

  }



  G4ThreeVector Next100FieldCage::GetActivePosition() const
  {
    return G4ThreeVector (0., 0., _active_zpos);
  }


  G4double Next100FieldCage::GetDistanceGateSapphireWindows() const
  {
    return _active_length + _buffer_length;
  }


}
