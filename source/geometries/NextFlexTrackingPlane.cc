// -----------------------------------------------------------------------------
//  nexus | NextFlexTrackingPlane.cc
//
//  * Info:         : NEXT-Flex Tracking Plane geometry for performance studies.
//  * Author        : <jmunoz@ific.uv.es>
//  * Creation date : January 2020
// -----------------------------------------------------------------------------

#include "NextFlexTrackingPlane.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "XenonGasProperties.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "CylinderPointSampler2020.h"
#include "GenericPhotosensor.h"
#include "PmtSD.h"
#include "Visibilities.h"

#include <G4UnitsTable.hh>
#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4SubtractionSolid.hh>
#include <G4TransportationManager.hh>
#include <G4RotationMatrix.hh>

#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4SDManager.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UserLimits.hh>
#include <Randomize.hh>


using namespace nexus;


NextFlexTrackingPlane::NextFlexTrackingPlane():
  BaseGeometry(),
  _mother_logic      (nullptr),
  _verbosity         (false),
  _visibility        (false),
  _msg               (nullptr),
  _wls_matName       ("TPB"),
  _SiPM_ANODE_dist   (10.  * mm),   // Distance from ANODE to SiPM surface
  _SiPM_sizeX        ( 1.3 * mm),   // Size X (width) of SiPMs
  _SiPM_sizeY        ( 1.3 * mm),   // Size Y (height) of SiPMs
  _SiPM_pitchX       (15.6 * mm),   // SiPMs pitch X
  _SiPM_pitchY       (15.6 * mm),   // SiPMs pitch Y
  _SiPM_bin          ( 1.  * us),   // SiPMs time bin size
  _copper_thickness  (12.  * cm),   // Thickness of the copper plate
  _teflon_thickness  ( 5.  * mm),   // Thickness of the teflon mask
  _teflon_hole_diam  ( 7.  * mm)    // Diameter of teflon mask holes
{

  // Messenger
  _msg = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextFlex geometry.");

  // Parametrized dimensions
  DefineConfigurationParameters();

  // Hard-wired dimensions & components
  _SiPM_case_thickness = 2. * mm;
  _wls_thickness       = 1. * um;

  // The SiPM
  _SiPM = new GenericPhotosensor("TP_SiPM", _SiPM_sizeX, _SiPM_sizeY, 
                                 _SiPM_case_thickness);

  // Initializing the geometry navigator (used in vertex generation)
  _geom_navigator =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
}



NextFlexTrackingPlane::~NextFlexTrackingPlane()
{
  delete _msg;
  delete _copper_gen;
  delete _SiPM;
}



void NextFlexTrackingPlane::DefineConfigurationParameters()
{
  // Verbosity
  _msg->DeclareProperty("tp_verbosity", _verbosity, "Verbosity");

  // Visibility
  _msg->DeclareProperty("tp_visibility", _visibility, "TRACKING_PLANE Visibility");

  // Copper dimensions
  G4GenericMessenger::Command& copper_thickness_cmd =
    _msg->DeclareProperty("tp_copper_thickness", _copper_thickness,
                          "Thickness of the TP Copper plate.");
  copper_thickness_cmd.SetParameterName("tp_copper_thickness", false);
  copper_thickness_cmd.SetUnitCategory("Length");
  copper_thickness_cmd.SetRange("tp_copper_thickness>=0.");

  // Teflon dimensions
  G4GenericMessenger::Command& teflon_thickness_cmd =
    _msg->DeclareProperty("tp_teflon_thickness", _teflon_thickness,
                          "Thickness of the TP teflon mask.");
  teflon_thickness_cmd.SetParameterName("tp_teflon_thickness", false);
  teflon_thickness_cmd.SetUnitCategory("Length");
  teflon_thickness_cmd.SetRange("tp_teflon_thickness>=0.");

  G4GenericMessenger::Command& teflon_hole_diam_cmd =
    _msg->DeclareProperty("tp_teflon_hole_diam", _teflon_hole_diam,
                          "Diameter of the TP teflon mask holes.");
  teflon_hole_diam_cmd.SetParameterName("tp_teflon_hole_diam", false);
  teflon_hole_diam_cmd.SetUnitCategory("Length");
  teflon_hole_diam_cmd.SetRange("tp_teflon_hole_diam>=0.");

  // UV shifting material
  _msg->DeclareProperty("tp_wls_mat", _wls_matName,
                        "TP UV wavelength shifting material name");

  // SiPMs
  G4GenericMessenger::Command& sipm_anode_dist_cmd =
    _msg->DeclareProperty("tp_sipm_anode_dist", _SiPM_ANODE_dist,
                          "Distance from tracking SiPMs to ANODE.");
  sipm_anode_dist_cmd.SetParameterName("tp_sipm_anode_dist", false);
  sipm_anode_dist_cmd.SetUnitCategory("Length");
  sipm_anode_dist_cmd.SetRange("tp_sipm_anode_dist>=0.");

  G4GenericMessenger::Command& sipm_sizeX_cmd =
    _msg->DeclareProperty("tp_sipm_sizeX", _SiPM_sizeX,
                          "SizeX of tracking SiPMs.");
  sipm_sizeX_cmd.SetParameterName("tp_sipm_sizeX", false);
  sipm_sizeX_cmd.SetUnitCategory("Length");
  sipm_sizeX_cmd.SetRange("tp_sipm_sizeX>0.");

  G4GenericMessenger::Command& sipm_sizeY_cmd =
    _msg->DeclareProperty("tp_sipm_sizeY", _SiPM_sizeY,
                          "SizeY of tracking SiPMs.");
  sipm_sizeY_cmd.SetParameterName("tp_sipm_sizeY", false);
  sipm_sizeY_cmd.SetUnitCategory("Length");
  sipm_sizeY_cmd.SetRange("tp_sipm_sizeY>0.");

  G4GenericMessenger::Command& sipm_pitchX_cmd =
    _msg->DeclareProperty("tp_sipm_pitchX", _SiPM_pitchX,
                          "PitchX of tracking SiPMs.");
  sipm_pitchX_cmd.SetParameterName("tp_sipm_pitchX", false);
  sipm_pitchX_cmd.SetUnitCategory("Length");
  sipm_pitchX_cmd.SetRange("tp_sipm_pitchX>0.");

  G4GenericMessenger::Command& sipm_pitchY_cmd =
    _msg->DeclareProperty("tp_sipm_pitchY", _SiPM_pitchY,
                          "PitchY of tracking SiPMs.");
  sipm_pitchY_cmd.SetParameterName("tp_sipm_pitchY", false);
  sipm_pitchY_cmd.SetUnitCategory("Length");
  sipm_pitchY_cmd.SetRange("tp_sipm_pitchY>0.");

  G4GenericMessenger::Command& sipm_bin_cmd =
    _msg->DeclareProperty("tp_sipm_time_binning", _SiPM_bin,
                          "Time bin size of SiPMs.");
  sipm_bin_cmd.SetParameterName("tp_sipm_time_binning", false);
  sipm_bin_cmd.SetUnitCategory("Time");
  sipm_bin_cmd.SetRange("tp_sipm_time_binning>=0.");
}



void NextFlexTrackingPlane::ComputeDimensions()
{
  _teflon_iniZ = _originZ - _SiPM_ANODE_dist - _SiPM_case_thickness;
  _copper_iniZ = _teflon_iniZ - _copper_thickness;

  // Generate SiPM positions
  GenerateSiPMpositions();
}



void NextFlexTrackingPlane::DefineMaterials()
{
  // Xenon
  _xenon_gas  = _mother_logic->GetMaterial();

  // Copper
  _copper_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

  // Teflon
  _teflon_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

  // UV shifting material
  if (_wls_matName == "NONE") {
    _wls_mat = _mother_logic->GetMaterial();
  }
  else if (_wls_matName == "TPB") {
    _wls_mat = MaterialsList::TPB();
    _wls_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
  }
  else if (_wls_matName == "TPH") {
    _wls_mat = MaterialsList::TPH();
    _wls_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::TPH());
  }
  else {
    G4Exception("[NextFlex]", "EnergyPlane::DefineMaterials()", FatalException,
                "Unknown UV shifting material. Valid options are NONE, TPB or TPH.");
  }
}



void NextFlexTrackingPlane::Construct()
{
  // Make sure that the pointer to the mother volume is actually defined
  if (!_mother_logic)
    G4Exception("[NextFlexTrackingPlane]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  // Verbosity
  if(_verbosity) {
    G4cout << G4endl << "*** NEXT-Flex Tracking Plane ..." << G4endl;
  }

  // Getting volumes dimensions based on parameters.
  ComputeDimensions();

  // Define materials.
  DefineMaterials();

  // Copper
  BuildCopper();

  // Teflon
  BuildTeflon();
}



void NextFlexTrackingPlane::BuildCopper()
{
  G4String copper_name = "TP_COPPER";

  G4double copper_posZ = _copper_iniZ + _copper_thickness/2.;

  G4Tubs* copper_solid =
    new G4Tubs(copper_name, 0., _diameter/2., _copper_thickness/2., 0, twopi);

  G4LogicalVolume* copper_logic =
    new G4LogicalVolume(copper_solid, _copper_mat, copper_name);

  G4VPhysicalVolume* copper_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., copper_posZ), copper_logic,
                      copper_name, _mother_logic, false, 0, _verbosity);

  // Visibility
  if (_visibility) copper_logic->SetVisAttributes(nexus::CopperBrown());
  else             copper_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  _copper_gen = new CylinderPointSampler2020(copper_phys);

  // Verbosity
  if (_verbosity) {
    G4cout << "* TP Copper Z positions: " << _copper_iniZ
           << " to " << _copper_iniZ + _copper_thickness << G4endl;
  }
}



void NextFlexTrackingPlane::BuildTeflon()
{
  /// The TEFLON ///
  G4String teflon_name = "TP_TEFLON";

  G4double teflon_posZ = _teflon_iniZ + _teflon_thickness/2.;

  G4Tubs* teflon_solid =
    new G4Tubs(teflon_name, 0., _diameter/2., _teflon_thickness/2., 0, twopi);

  G4LogicalVolume* teflon_logic =
    new G4LogicalVolume(teflon_solid, _teflon_mat, teflon_name);

  // Adding the teflon optical surface
  G4OpticalSurface* teflon_optSurf = 
    new G4OpticalSurface(teflon_name, unified, ground, dielectric_metal);
  teflon_optSurf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface(teflon_name, teflon_logic, teflon_optSurf);


  /// The UV WLS in TEFLON ///
  G4String teflon_wls_name = "TP_TEFLON_WLS";

  G4double teflon_wls_posZ = _teflon_thickness/2. - _wls_thickness/2.;

  G4Tubs* teflon_wls_solid =
    new G4Tubs(teflon_wls_name, 0., _diameter/2., _wls_thickness/2., 0, twopi);

  G4LogicalVolume* teflon_wls_logic =
    new G4LogicalVolume(teflon_wls_solid, _wls_mat, teflon_wls_name);

  G4VPhysicalVolume* teflon_wls_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., teflon_wls_posZ), teflon_wls_logic,
                      teflon_wls_name, teflon_logic, false, 0, _verbosity);

  // Adding the WLS optical surface
  G4OpticalSurface* teflon_wls_optSurf =
    new G4OpticalSurface("TEFLON_WLS_OPSURF", glisur, ground,
                         dielectric_dielectric, .01);

  new G4LogicalBorderSurface("TEFLON_WLS_GAS_OPSURF", teflon_wls_phys,
                             _neigh_gas_phys, teflon_wls_optSurf);
  new G4LogicalBorderSurface("GAS_TEFLON_WLS_OPSURF", _neigh_gas_phys,
                             teflon_wls_phys, teflon_wls_optSurf);

  /// Adding the SiPMs ///

  // The SiPM
  G4LogicalVolume* SiPM_logic = BuildSiPM();

  // teflon wls hole
  G4String wls_hole_name   = "TP_TEFLON_WLS_HOLE";
  G4double wls_hole_diam   = _teflon_hole_diam;
  G4double wls_hole_length = _wls_thickness;

  G4Tubs* wls_hole_solid =
    new G4Tubs(wls_hole_name, 0., wls_hole_diam/2., wls_hole_length/2., 0, twopi);

  G4LogicalVolume* wls_hole_logic = 
    new G4LogicalVolume(wls_hole_solid, _xenon_gas, wls_hole_name);


  // teflon hole
  G4String hole_name   = "TP_TEFLON_HOLE";
  G4double hole_diam   = _teflon_hole_diam;
  G4double hole_length = _teflon_thickness - _wls_thickness;
  G4double hole_posz   = -_teflon_thickness/2. + hole_length/2.;

  G4Tubs* hole_solid =
    new G4Tubs(hole_name, 0., hole_diam/2., hole_length/2., 0, twopi);

  G4LogicalVolume* hole_logic = 
    new G4LogicalVolume(hole_solid, _xenon_gas, hole_name);

  // Placing the SiPM into the teflon hole
  G4double SiPM_pos_z = - hole_length/2. + _SiPM_case_thickness / 2.;

  new G4PVPlacement(0, G4ThreeVector(0., 0., SiPM_pos_z), SiPM_logic,
                    SiPM_logic->GetName(), hole_logic, false, 0, true);

  // Replicating the teflon & wls-teflon holes
  for (G4int i=0; i<_num_SiPMs; i++) {
    G4int SiPM_id = _first_sensor_id + i;

    G4ThreeVector hole_pos = _SiPM_positions[i];
    hole_pos.setZ(hole_posz);
    new G4PVPlacement(nullptr, hole_pos, hole_logic, hole_name,
                      teflon_logic, true, SiPM_id, false);

    G4ThreeVector wls_hole_pos = _SiPM_positions[i];
    new G4PVPlacement(nullptr, wls_hole_pos, wls_hole_logic, wls_hole_name,
                      teflon_wls_logic, true, SiPM_id, false);

    //if (_verbosity) G4cout << "* SiPM " << SiPM_id << " position: " 
    //                       << hole_pos << G4endl;
  }

  // Placing the overall teflon sub-system
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., teflon_posZ), teflon_logic,
                    teflon_name, _mother_logic, false, 0, _verbosity);

  /// Verbosity ///
  if (_verbosity) {
    G4cout << "* Teflon Z positions: " << _teflon_iniZ
           << " to " << _teflon_iniZ + _teflon_thickness << G4endl;
    G4cout << "* SiPM Z positions: " << _teflon_iniZ 
           << " to " << _teflon_iniZ + _SiPM_case_thickness << G4endl;
  } 


  /// Visibilities ///
  if (_visibility) {
    G4VisAttributes light_blue_col = nexus::LightBlue();
    teflon_logic    ->SetVisAttributes(light_blue_col);
    teflon_wls_logic->SetVisAttributes(G4VisAttributes::Invisible);
    hole_logic      ->SetVisAttributes(G4VisAttributes::Invisible);
    wls_hole_logic ->SetVisAttributes(G4VisAttributes::Invisible);
  }
  else {
    teflon_logic    ->SetVisAttributes(G4VisAttributes::Invisible);
    teflon_wls_logic->SetVisAttributes(G4VisAttributes::Invisible);
    hole_logic      ->SetVisAttributes(G4VisAttributes::Invisible);
    wls_hole_logic  ->SetVisAttributes(G4VisAttributes::Invisible);
  }

}



G4LogicalVolume* NextFlexTrackingPlane::BuildSiPM()
{
  /// Constructing the TP SiPM ///
  // Optical Properties of the sensor
  G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
  G4double energy[]       = {0.2 * eV, 11.5 * eV};
  G4double reflectivity[] = {0.0     ,  0.0};
  G4double efficiency[]   = {1.0     ,  1.0};
  photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
  photosensor_mpt->AddProperty("EFFICIENCY",   energy, efficiency,   2);
  _SiPM->SetOpticalProperties(photosensor_mpt);

  // Set WLS coating
  _SiPM->SetWithWLSCoating(true);

  // Set mother depth & naming order
  _SiPM->SetSensorDepth(1);
  _SiPM->SetMotherDepth(2);
  _SiPM->SetNamingOrder(1);

  // Construct
  _SiPM->Construct();

  return _SiPM->GetLogicalVolume();
}



// Function that computes and stores the XY positions of SiPMs in the copper plate
void NextFlexTrackingPlane::GenerateSiPMpositions()
{
  // Maximum radius to place SiPMs
  // Lower than diameter to prevent SiPMs being partially out.
  G4double max_radius = _diameter/2. - _teflon_hole_diam/2.;
  G4double ini_pos_XY = - _diameter/2.;

  G4int num_rows    = (G4int) (max_radius * 2 / _SiPM_pitchY);
  G4int num_columns = (G4int) (max_radius * 2 / _SiPM_pitchX);

  for (G4int num_row=0; num_row<num_rows; num_row++) {
    G4double posY = ini_pos_XY + num_row * _SiPM_pitchY;

    for (G4int num_column=0; num_column<num_columns; num_column++) {
      G4double posX = ini_pos_XY + num_column * _SiPM_pitchX;

      G4double radius = pow (pow(posX, 2.) + pow(posY, 2.), 0.5);

      if (radius <= max_radius)
        _SiPM_positions.push_back(G4ThreeVector(posX, posY, 0.));
    }
  }

  _num_SiPMs = _SiPM_positions.size();

  if (_verbosity) {
    G4cout << "* TP num SiPM rows   : " << num_rows    << G4endl;
    G4cout << "* TP num SiPM columns: " << num_columns << G4endl;
    G4cout << "* Total num SiPMs    : " << _num_SiPMs  << G4endl;
  }
}



G4ThreeVector NextFlexTrackingPlane::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "TP_COPPER") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex       = _copper_gen->GenerateVertex("VOLUME");
      VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else {
    G4Exception("[NextNew]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }

  return vertex;
}
