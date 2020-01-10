// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <jmunoz@ific.uv.es>
//  Created: 2 Mar 2012
//
//  Copyright (c) 2012-2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100InnerElements.h"

#include "Next100FieldCage.h"
#include "Next100EnergyPlane.h"
#include "Next100TrackingPlane.h"
#include "IonizationSD.h"
#include "PmtSD.h"
#include "UniformElectricDriftField.h"
#include "OpticalMaterialProperties.h"
#include "MaterialsList.h"
#include "XenonGasProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4Region.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>
#include <G4TransportationManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

using namespace CLHEP;


namespace nexus {


  Next100InnerElements::Next100InnerElements():
    _max_step_size (1.*mm),
    _active_diam   (106.9 * cm),
    _active_length (130. * cm),
    _windows_end_z (-704.349 * mm), // It's the position in gas where the sapphire windows end. To be read from Next100EnergyPlane.cc, once TPB is added to sapphire windows
    _trk_displ (13.36 * cm),
    _el_gap_length (.5 * cm),
    _grid_thickn (.1 * mm),
    _el_grid_transparency (.88),
    _cath_grid_transparency (.98),
    _elfield(0), // EL field ON or OFF
    _ELtransv_diff (0. * mm/sqrt(cm)),
    _ELlong_diff (0. * mm/sqrt(cm)),
    _ELelectric_field (34.5*kilovolt/cm),
    _drift_transv_diff (1. * mm/sqrt(cm)),
    _drift_long_diff (.3 * mm/sqrt(cm)),
    _anode_quartz_thickness (3 *mm),
    _anode_quartz_diam (110.*cm),
    _tpb_thickness (1.*micrometer),
    _ito_transparency (.90),
    _ito_thickness (_grid_thickn),
    _el_table_binning(1.*mm),
    _el_table_point_id(-1),
    _el_table_index(0),
    _grids_visibility(false) // Visibility of grids
  {
    _el_gap_posz = (160.*cm / 2.) - _trk_displ;

    _cathode_pos_z = _el_gap_posz - _el_gap_length/2. - _active_length - _grid_thickn/2.;
    _buffer_length =
      _cathode_pos_z - _grid_thickn/2. - _windows_end_z; // from the end of the cathode to surface of sapphire windows // It's 129.9 mm

    // Field Cage
    _field_cage = new Next100FieldCage();

    // Energy Plane
    _energy_plane = new Next100EnergyPlane();

    // Tracking Plane
    _tracking_plane = new Next100TrackingPlane();

    // Define a new category
    new G4UnitDefinition("kilovolt/cm","kV/cm","Electric field", kilovolt/cm);
    new G4UnitDefinition("mm/sqrt(cm)","mm/sqrt(cm)","Diffusion", mm/sqrt(cm));

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");

    _msg->DeclareProperty("grids_vis", _grids_visibility, "Grids Visibility");

    G4GenericMessenger::Command& step_cmd =
      _msg->DeclareProperty("max_step_size", _max_step_size, "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");

    _msg->DeclareProperty("elfield", _elfield,
			  "True if the EL field is on (full simulation), false if it's not (parametrized simulation.");

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

    G4GenericMessenger::Command& El_field_cmd =
      _msg->DeclareProperty("EL_field", _ELelectric_field,
			    "Electric field in the EL region");
    El_field_cmd.SetParameterName("EL_field", true);
    El_field_cmd.SetUnitCategory("Electric field");

     // Calculate vertices for EL table generation
    G4GenericMessenger::Command& pitch_cmd =
      _msg->DeclareProperty("el_table_binning", _el_table_binning,
			    "Binning of EL lookup tables.");
    pitch_cmd.SetUnitCategory("Length");
    pitch_cmd.SetParameterName("el_table_binning", false);
    pitch_cmd.SetRange("el_table_binning>0.");

    _msg->DeclareProperty("el_table_point_id", _el_table_point_id, "");
  }



  void Next100InnerElements::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }



  void Next100InnerElements::Construct()
  {
    // Reading material
    _gas = _mother_logic->GetMaterial();
    _pressure =    _gas->GetPressure();
    _temperature = _gas->GetTemperature();

    G4double z = _el_gap_posz - _el_gap_length/2.;
    // 0.1 * mm is added to avoid very small negative numbers in drift lengths
    SetELzCoord(z + 0.1 * mm);

    // 0.5 * mm is added because ie- in EL table generation must start inside the volume, not on border
    z = z + .5*mm;
    G4double max_radius = floor(_active_diam/2./_el_table_binning)*_el_table_binning;
    CalculateELTableVertices(max_radius, _el_table_binning, z);


    // Field Cage
    _field_cage->SetMotherLogicalVolume(_mother_logic);
    _field_cage->Construct();
    G4LogicalVolume* field_cage_logic = _field_cage->GetLogicalVolume();
    G4ThreeVector    field_cage_pos =   _field_cage->GetPosition();
    new G4PVPlacement(0, field_cage_pos, field_cage_logic,
		      "FIELD_CAGE", _mother_logic, false, 0);

    // EL Region
    BuildELRegion();

    BuildAnodePlate();

    // Cathode Grid
    BuildCathodeGrid();

    // ACTIVE region
    BuildActive();

    // Buffer region
    BuildBuffer();

    // Energy Plane
    _energy_plane->SetLogicalVolume(_mother_logic);
    _energy_plane->SetSapphireSurfaceZPos(1000. * mm); // placeholder, to be changed after field cage implementation.
    _energy_plane->Construct();

    // Tracking Plane
    _tracking_plane->SetLogicalVolume(_mother_logic);
    _tracking_plane->SetELzCoord(GetELzCoord());
    _tracking_plane->Construct();

  }



  Next100InnerElements::~Next100InnerElements()
  {
    delete _field_cage;
    delete _energy_plane;
    delete _tracking_plane;
    delete _xenon_gen;
    delete _active_gen;
    delete _buffer_gen;
    delete _anode_quartz_gen;
    delete _cathode_gen;
  }



  void Next100InnerElements::BuildELRegion()
  {

    /// EL GAP
    G4double el_gap_diam = _active_diam;

    G4Tubs* el_gap_solid =
      new G4Tubs("EL_GAP", 0., el_gap_diam/2., _el_gap_length/2., 0, twopi);

    G4LogicalVolume* el_gap_logic =
      new G4LogicalVolume(el_gap_solid, _gas, "EL_GAP");

    new G4PVPlacement(0, G4ThreeVector(0., 0., _el_gap_posz), el_gap_logic,
		      "EL_GAP", _mother_logic, false, 0);

    // G4cout << "EL GAP starts in " << _el_gap_posz - _el_gap_length/2.
    // 	   << " and ends in " << _el_gap_posz + _el_gap_length/2. << G4endl;

 if (_elfield) {
    // Define EL electric field
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    el_field->SetCathodePosition(-(_el_gap_posz - _el_gap_length/2.) + GetELzCoord());
    el_field->SetAnodePosition  (-(_el_gap_posz + _el_gap_length/2.) + GetELzCoord());
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
    G4double grid_diam = _active_diam;

    G4double posz1 = - _el_gap_length/2. + _grid_thickn/2.;
    //   G4double posz2 =  _el_gap_length/2. - _grid_thickn/2.;

    _el_grid_ref_z = posz1;

    G4Tubs* diel_grid_solid =
      new G4Tubs("EL_GRID", 0., grid_diam/2., _grid_thickn/2., 0, twopi);

    G4LogicalVolume* diel_grid_logic =
      new G4LogicalVolume(diel_grid_solid, fgrid_mat, "EL_GRID");

    new G4PVPlacement(0, G4ThreeVector(0.,0.,posz1), diel_grid_logic, "EL_GRID_1",
		      el_gap_logic, false, 0, false);
    // new G4PVPlacement(0, G4ThreeVector(0.,0.,posz2), diel_grid_logic, "EL_GRID_2",
    // 		      el_gap_logic, false, 1, true); // not there, if quartz plate

    /// Visibilities
    if (_grids_visibility) {
      G4VisAttributes light_blue = nexus::LightBlue();
      light_blue.SetForceSolid(true);
      el_gap_logic->SetVisAttributes(light_blue);
      // grids are white
    } else {
      el_gap_logic->SetVisAttributes(G4VisAttributes::Invisible);
      diel_grid_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

 void Next100InnerElements::BuildAnodePlate()
  {
     // Materials: quartz
    G4Material* quartz =  MaterialsList::FusedSilica();
    quartz->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
    // TPB coating
    G4Material* tpb = MaterialsList::TPB();
    tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
     //ITO coating
    G4Material* ito = MaterialsList::ITO();
    ito->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeFusedSilica(_ito_transparency, _ito_thickness));

    G4Tubs* anode_solid =
      new G4Tubs("ANODE_PLATE", 0., _anode_quartz_diam/2. , _anode_quartz_thickness/2., 0, twopi);
    G4LogicalVolume* anode_logic =
      new G4LogicalVolume(anode_solid, quartz, "ANODE_PLATE");

    G4double pos_anode_z = _el_gap_posz + _el_gap_length/2. + _anode_quartz_thickness/2. + 0.1*mm; // 0.1 mm is needed because EL is produced only if the PostStepVolume is GAS material.

    new G4PVPlacement(0, G4ThreeVector(0., 0., pos_anode_z), anode_logic,
 		      "ANODE_PLATE", _mother_logic, false, 0, false);

    // G4cout << "Anode plate starts in " << _pos_z_anode - _anode_quartz_thickness/2. << " and ends in " <<
    //   _pos_z_anode + _anode_quartz_thickness/2. << G4endl;

    G4Tubs* tpb_anode_solid =
      new G4Tubs("TPB_ANODE", 0., _anode_quartz_diam/2. , _tpb_thickness/2., 0, twopi);
    G4LogicalVolume* tpb_anode_logic =
      new G4LogicalVolume(tpb_anode_solid, tpb, "TPB_ANODE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., -_anode_quartz_thickness/2.+_tpb_thickness/2.), tpb_anode_logic,
 		      "TPB_ANODE", anode_logic, false, 0, false);

    G4Tubs* ito_anode_solid =
      new G4Tubs("ITO_ANODE", 0., _anode_quartz_diam/2. , _ito_thickness/2., 0, twopi);
    G4LogicalVolume* ito_anode_logic =
      new G4LogicalVolume(ito_anode_solid, ito, "ITO_ANODE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., +_anode_quartz_thickness/2.- _ito_thickness/2.), ito_anode_logic,
  			"ITO_ANODE", anode_logic, false, 0, false);


    G4double only_quartz_thickn = _anode_quartz_thickness - _tpb_thickness - _ito_thickness;
    G4double only_quartz_z_pos  = pos_anode_z + _anode_quartz_thickness/2. - _ito_thickness - only_quartz_thickn /2.;
    _anode_quartz_gen =
      new CylinderPointSampler(0., only_quartz_thickn, _anode_quartz_diam/2.,
                               0., G4ThreeVector (0., 0., only_quartz_z_pos));


    if (_grids_visibility) {
      G4VisAttributes anode_col = nexus::Red();
      anode_col.SetForceSolid(true);
      anode_logic->SetVisAttributes(anode_col);
      G4VisAttributes tpb_col = nexus::DarkGreen();
      //tpb_col.SetForceSolid(true);
      tpb_anode_logic->SetVisAttributes(tpb_col);
    } else {
      anode_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_anode_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

  }


  void Next100InnerElements::BuildCathodeGrid()
  {

    G4Material* fgrid_mat = MaterialsList::FakeDielectric(_gas, "cath_grid_mat");
    fgrid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature,
									      _cath_grid_transparency, _grid_thickn));

    // Dimensions & position
    G4double grid_diam = _active_diam;
    // G4double posz = _el_grid_ref_z - _grid_thickn - _active_length;
    G4double posz = _el_gap_posz - _el_gap_length/2. - _active_length - _grid_thickn/2. ;

    //G4cout << G4endl << "Cathode Grid posz: " << posz/cm << G4endl;

    // Building the grid
    G4Tubs* diel_grid_solid = new G4Tubs("CATH_GRID", 0., grid_diam/2., _grid_thickn/2., 0, twopi);

    G4LogicalVolume* diel_grid_logic = new G4LogicalVolume(diel_grid_solid, fgrid_mat, "CATH_GRID");

    new G4PVPlacement(0, G4ThreeVector(0., 0., posz), diel_grid_logic, "CATH_GRID",
		      _mother_logic, false, 0, false);

    // Vertex generator
     _cathode_gen =
      new CylinderPointSampler(0., _grid_thickn, grid_diam/2.,
			       0., G4ThreeVector (0., 0., posz));

    /// Visibilities
    // Grid is white
    if (!_grids_visibility) {
      diel_grid_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

  }



  void Next100InnerElements::BuildActive()
  {
    //    G4double active_posz = _el_grid_ref_z - _grid_thickn/2. - _active_length/2.;
    _active_posz = _el_gap_posz - _el_gap_length/2. - _active_length/2.;

    // G4cout << "ACTIVE starts in " << active_posz - _active_length/2.
    // 	   << " and ends in " << active_posz + _active_length/2. << G4endl;

    G4Tubs* active_solid = new G4Tubs("ACTIVE",  0., _active_diam/2., _active_length/2., 0, twopi);

    G4LogicalVolume* active_logic = new G4LogicalVolume(active_solid, _gas, "ACTIVE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., _active_posz), active_logic,
		      "ACTIVE", _mother_logic, false, 0, false);

    // Limit the step size in this volume for better tracking precision
    active_logic->SetUserLimits(new G4UserLimits(_max_step_size));

    // Set the volume as an ionization sensitive detector
    IonizationSD* ionisd = new IonizationSD("/NEXT100/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    //Define a drift field for this volume
    UniformElectricDriftField* field = new UniformElectricDriftField();
    field->SetCathodePosition(-(_active_posz - _active_length/2.) + GetELzCoord());
    field->SetAnodePosition(-(_active_posz + _active_length/2.) + GetELzCoord());
    field->SetDriftVelocity(1. * mm/microsecond);
    field->SetTransverseDiffusion(_drift_transv_diff);
    field->SetLongitudinalDiffusion(_drift_long_diff);
    G4Region* drift_region = new G4Region("DRIFT");
    drift_region->SetUserInformation(field);
    drift_region->AddRootLogicalVolume(active_logic);


    /// Visibilities
    active_logic->SetVisAttributes(G4VisAttributes::Invisible);


    // VERTEX GENERATOR
    _active_gen = new CylinderPointSampler(0., _active_length, _active_diam/2.,
					   0., G4ThreeVector (0., 0., _active_posz));
  }

void Next100InnerElements::BuildBuffer()
  {

    G4double buffer_posz =_el_gap_posz - _el_gap_length/2. - _active_length - _grid_thickn - _buffer_length/2.;
    G4Tubs* buffer_solid =
      new G4Tubs("BUFFER",  0., _active_diam/2.,
		 _buffer_length /2., 0, twopi);

    // G4cout << "Buffer (gas) starts in " << buffer_posz - _buffer_length/2. << " and ends in "
    // 	   << buffer_posz + _buffer_length/2. << G4endl;
    G4LogicalVolume* buffer_logic =
      new G4LogicalVolume(buffer_solid, _gas, "BUFFER");
    new G4PVPlacement(0, G4ThreeVector(0., 0., buffer_posz), buffer_logic,
		      "BUFFER", _mother_logic, false, 0, false);

     // Set the volume as an ionization sensitive detector
    IonizationSD* buffsd = new IonizationSD("/NEXT100/BUFFER");
    buffsd->IncludeInTotalEnergyDeposit(false);
    buffer_logic->SetSensitiveDetector(buffsd);
    G4SDManager::GetSDMpointer()->AddNewDetector(buffsd);

    // VERTEX GENERATOR
    _buffer_gen =
      new CylinderPointSampler(0., _buffer_length, _active_diam/2.,
			       0., G4ThreeVector (0., 0., buffer_posz));

    // VERTEX GENERATOR FOR ALL XENON
    G4double xenon_posz = (_buffer_length * buffer_posz +
			   _active_length * _active_posz +
			   _grid_thickn * _cathode_pos_z +
			   _grid_thickn * _el_grid_ref_z +
			   _el_gap_length * _el_gap_posz) / (_buffer_length +
							     _active_length +
							     2 * _grid_thickn +
							     _el_gap_length);
    G4double xenon_len = _buffer_length + _active_length + _grid_thickn;
    _xenon_gen =
      new CylinderPointSampler(0., xenon_len, _active_diam/2.,
			       0., G4ThreeVector (0., 0., xenon_posz));

  }

  G4ThreeVector Next100InnerElements::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // Field Cage region
    if (region == "FIELD_CAGE") {
      vertex = _field_cage->GenerateVertex(region);
    }
    else if (region == "CATHODE") {
      vertex = _cathode_gen->GenerateVertex("BODY_VOL");
    }
    //All xenon
    else if (region == "XENON") {
      G4Navigator *geom_navigator =
	G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
      G4String volume_name;
      do {
	vertex = _xenon_gen->GenerateVertex("BODY_VOL");
	G4ThreeVector glob_vtx(vertex);
	CalculateGlobalPos(glob_vtx);
	volume_name =
	  geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false)->GetName();
      } while (volume_name == "CATH_GRID" || volume_name == "EL_GRID");
    }
    // Active region
    else if (region == "ACTIVE") {
      vertex = _active_gen->GenerateVertex("BODY_VOL");
    }
    // Buffer
    else if (region == "BUFFER") {
      vertex = _buffer_gen->GenerateVertex("BODY_VOL");
    }
    // Anode plate
    else if (region == "ANODE_QUARTZ") {
      vertex = _anode_quartz_gen->GenerateVertex("BODY_VOL");
    }

    // Energy Plane regions
    else if ((region == "ENERGY_COPPER_PLATE") ||
             (region == "SAPPHIRE_WINDOW") ||
             (region == "OPTICAL_PAD") ||
	     (region == "PMT") ||
             (region == "PMT_BODY") ||
	     (region == "INTERNAL_PMT_BASE") ||
	     (region == "EXTERNAL_PMT_BASE")) {
      vertex = _energy_plane->GenerateVertex(region);
    }

    // Tracking Plane regions
    else if ((region == "TRK_SUPPORT") ||
             (region == "DICE_BOARD")) {
      vertex = _tracking_plane->GenerateVertex(region);
    }
    // Vertex decided by user
    else if (region == "AD_HOC") {
      vertex =
	G4ThreeVector(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
    }

    // Axial port --- temporary, until correct copper thickness is known
    else if (region == "AXIAL_PORT") {
      vertex = G4ThreeVector(0., 0., _windows_end_z);
    }
    else if (region == "EL_TABLE") {

      unsigned int i = _el_table_point_id + _el_table_index;

      if (i == (_table_vertices.size()-1)) {
        G4Exception("[Next100InnerElements]", "GenerateVertex()",
          RunMustBeAborted, "Reached last event in EL lookup table.");
      }

      try {
        vertex = _table_vertices.at(i);
        _el_table_index++;
      }
      catch (const std::out_of_range& oor) {
        G4Exception("[Next100InnerElements]", "GenerateVertex()", FatalErrorInArgument, "EL lookup table point out of range.");
      }
    }
    else {
      G4Exception("[Next100InnerElements]", "GenerateVertex()", FatalException,
        "Unknown vertex generation region!");
    }

    return vertex;
  }


  void Next100InnerElements::CalculateELTableVertices(G4double radius, G4double binning, G4double z)
  {
    // Calculate the xyz positions of the points of an EL lookup table
    // (arranged as a square grid) given a certain binning

    G4ThreeVector xyz(0., 0., z);

    G4int imax = floor(2*radius/binning); // maximum bin number (minus 1)

    for (int i=0; i<imax+1; i++) { // Loop through the x bins

      xyz.setX(-radius + i * binning); // x position

      for (int j=0; j<imax+1; j++) { // Loop through the y bins

        xyz.setY(-radius + j * binning); // y position

        // Store the point if it is inside the active volume defined by the
        // field cage (of circular cross section). Discard it otherwise.
        if (sqrt(xyz.x()*xyz.x()+xyz.y()*xyz.y()) <= radius)
          _table_vertices.push_back(xyz);
      }

    }
  }

} // end namespace nexus
