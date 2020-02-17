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
#include <G4UserLimits.hh>
#include <Randomize.hh>


using namespace nexus;


NextFlexTrackingPlane::NextFlexTrackingPlane():
  BaseGeometry(),
  _verbosity         (false),
  _visibility        (false),
  _msg               (nullptr),
  _wls_matName       ("TPB"),
  _SiPM_ANODE_dist   (10. * mm),   // Distance from ANODE to SiPM surface
  _SiPM_size         ( 1. * mm),   // Size of SiPMs
  _SiPM_pitchX       (15. * mm),   // SiPMs pitch X
  _SiPM_pitchY       (15. * mm),   // SiPMs pitch Y
  _SiPM_bin          ( 1. * us),   // SiPMs time bin size
  _copper_thickness  (12. * cm)    // Thickness of the copper plate
{

  // Messenger
  _msg = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextFlex geometry.");

  // Parametrized dimensions
  DefineConfigurationParameters();

  // Hard-wired dimensions & components
  _teflon_thickness    =  5. * mm;
  _wls_thickness       =  1. * um;
  _SiPM_thickness      = 10. * um;
  _SiPM_case_thickness =  1. * mm;


  // Initializing the geometry navigator (used in vertex generation)
  _geom_navigator =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
}


NextFlexTrackingPlane::~NextFlexTrackingPlane()
{
  delete _msg;
  delete _copper_gen;
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

  // UV shifting material
  _msg->DeclareProperty("tp_wls_mat", _wls_matName,
                        "TP UV wavelength shifting material name");

  // SiPMs
  G4GenericMessenger::Command& sipm_size_cmd =
    _msg->DeclareProperty("tp_sipm_size", _SiPM_size,
                          "Size of tracking SiPMs.");
  sipm_size_cmd.SetParameterName("tp_sipm_size", false);
  sipm_size_cmd.SetUnitCategory("Length");
  sipm_size_cmd.SetRange("tp_sipm_size>0.");

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
  _SiPM_iniZ   = _originZ - _SiPM_ANODE_dist - _SiPM_case_thickness;

  _teflon_iniZ = _SiPM_iniZ - _teflon_thickness;

  _copper_iniZ = _teflon_iniZ - _copper_thickness;

  // Generate SiPM positions
  GenerateSiPMpositions();
}


void NextFlexTrackingPlane::DefineMaterials()
{
  // Xenon
  _xenon_gas     = _mother_logic->GetMaterial();

  // Copper
  _copper_mat    = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

  // SiPM case
  _SiPM_case_mat = MaterialsList::Epoxy();
  _SiPM_case_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GlassEpoxy());

  // SiPM
  _SiPM_mat      = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

  // Teflon
  _teflon_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  _teflon_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

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

  // SiPMs
  BuildSiPMs();
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
  /// The teflon ///
  G4String teflon_name = "TP_TEFLON";

  G4double teflon_posZ = _teflon_iniZ + _teflon_thickness/2.;

  G4Tubs* teflon_solid =
    new G4Tubs(teflon_name, 0., _diameter/2., _teflon_thickness/2., 0, twopi);

  // Make SiPM holes
  //teflon_solid = dynamic_cast<G4Tubs*> (MakeSiPMholes(teflon_solid));
  //teflon_solid = (G4Tubs*) MakeSiPMholes(teflon_solid);

  G4LogicalVolume* teflon_logic =
    new G4LogicalVolume(teflon_solid, _teflon_mat, teflon_name);

  //G4VPhysicalVolume* teflon_phys =
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., teflon_posZ), teflon_logic,
                    teflon_name, _mother_logic, false, 0, _verbosity);

  // Visibility
  if (_visibility) {
    G4VisAttributes light_blue_col = nexus::LightBlue();
    //light_blue_col.SetForceSolid(true);
    teflon_logic->SetVisAttributes(light_blue_col);
  }
  else teflon_logic->SetVisAttributes(G4VisAttributes::Invisible);


  /// The UV wavelength Shifter in TEFLON ///
  G4String teflon_wls_name = "TP_TEFLON_WLS";

  G4double teflon_wls_posZ = _teflon_thickness/2. - _wls_thickness/2.;

  G4Tubs* teflon_wls_solid =
    new G4Tubs(teflon_wls_name, 0., _diameter/2., _wls_thickness/2., 0, twopi);

  // Make SiPM holes
  //teflon_wls_solid = dynamic_cast<G4Tubs*> (MakeSiPMholes(teflon_wls_solid));
  //teflon_wls_solid = (G4Tubs*) MakeSiPMholes(teflon_wls_solid);

  G4LogicalVolume* teflon_wls_logic =
    new G4LogicalVolume(teflon_wls_solid, _wls_mat, teflon_wls_name);

  //G4VPhysicalVolume* teflon_wls_phys =
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., teflon_wls_posZ), teflon_wls_logic,
                    teflon_wls_name, teflon_logic, false, 0, _verbosity);

  // Visibility
  teflon_wls_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Optical surface
  G4OpticalSurface* teflon_wls_optSurf = new G4OpticalSurface("teflon_wls_optSurf",
                                                              glisur, ground,
                                                              dielectric_dielectric, .01);

  new G4LogicalSkinSurface(teflon_wls_name, teflon_wls_logic, teflon_wls_optSurf);


  /// Verbosity ///
  if (_verbosity) {
    G4cout << "* Teflon Z positions: " << _teflon_iniZ
           << " to " << _teflon_iniZ + _teflon_thickness << G4endl;
  } 
}


void NextFlexTrackingPlane::BuildSiPMs()
{
  /// TP SiPM case ///
  G4String tp_SiPM_case_name = "TP_SiPM_CASE";

  G4double tp_SiPM_case_posZ = _SiPM_iniZ + _SiPM_case_thickness/2.;

  G4Box* tp_SiPM_case_solid =
    new G4Box(tp_SiPM_case_name, _SiPM_size/2., _SiPM_size/2., _SiPM_case_thickness/2.);

  G4LogicalVolume* tp_SiPM_case_logic =
    new G4LogicalVolume(tp_SiPM_case_solid, _SiPM_case_mat, tp_SiPM_case_name);

  // Visibility
  if (_visibility) {
    G4VisAttributes grey_col = nexus::LightGrey();
    grey_col.SetForceSolid(true);
    tp_SiPM_case_logic->SetVisAttributes(grey_col);
  }
  else tp_SiPM_case_logic->SetVisAttributes(G4VisAttributes::Invisible);


  /// The UV wavelength Shifter in SiPMs ///
  G4String tp_SiPM_wls_name = "TP_SiPM_WLS";

  G4double tp_SiPM_wls_posZ = _SiPM_case_thickness/2. - _wls_thickness/2.;

  G4Box* tp_SiPM_wls_solid =
    new G4Box(tp_SiPM_wls_name, _SiPM_size/2., _SiPM_size/2., _wls_thickness/2.);

  G4LogicalVolume* tp_SiPM_wls_logic =
    new G4LogicalVolume(tp_SiPM_wls_solid, _wls_mat, tp_SiPM_wls_name);

  //G4VPhysicalVolume* tp_SiPM_wls_phys =
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., tp_SiPM_wls_posZ), tp_SiPM_wls_logic,
                    tp_SiPM_wls_name, tp_SiPM_case_logic, false, 0, _verbosity);

  // Optical surface
  G4OpticalSurface* tp_SiPM_wls_optSurf =
    new G4OpticalSurface("tp_SiPM_wls_optSurf", glisur, ground, dielectric_dielectric, .01);

  new G4LogicalSkinSurface(tp_SiPM_wls_name, tp_SiPM_wls_logic, tp_SiPM_wls_optSurf);

  // Visibility
  tp_SiPM_wls_logic->SetVisAttributes(G4VisAttributes::Invisible);


  /// TP SiPM ///
  G4String tp_SiPM_name = "TP_SiPM";

  // Distance between wls and photosensor of SiPMs to avoid contiguous Skin Optical Surfaces.
  // Check if this affect the overall detection efficiency of SiPMs as refractive index of SiPM case
  // could be considered twice.
  G4double gap_wls_phot = 5 * um;

  G4double tp_SiPM_posZ = _SiPM_case_thickness/2. - _wls_thickness
                          - gap_wls_phot - _SiPM_thickness/2.;

  G4Box* tp_SiPM_solid =
    new G4Box(tp_SiPM_name, _SiPM_size/2., _SiPM_size/2., _SiPM_thickness/2.);

  G4LogicalVolume* tp_SiPM_logic =
    new G4LogicalVolume(tp_SiPM_solid, _SiPM_mat, tp_SiPM_name);

  //G4VPhysicalVolume* tp_SiPM_phys =
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., tp_SiPM_posZ), tp_SiPM_logic,
                    tp_SiPM_name, tp_SiPM_case_logic, false, 0, _verbosity);

  // Visibility
  tp_SiPM_logic->SetVisAttributes(G4VisAttributes::Invisible);

  /// SiPM Optical properties
  // (Same than SIPMSensl used in NEW)
  G4MaterialPropertiesTable* tp_SiPM_optProp = new G4MaterialPropertiesTable();

  const G4int entries = 21;
  G4double energies[entries] = {
    1.54980241262 * eV, 1.59979603883 * eV, 1.65312257346 * eV,
    1.71012680013 * eV, 1.77120275727 * eV, 1.8368028594  * eV,
    1.90744912322 * eV, 1.98374708815 * eV, 2.06640321682 * eV,
    2.15624683494 * eV, 2.25425805471 * eV, 2.36160367637 * eV,
    2.47968386018 * eV, 2.61019353704 * eV, 2.75520428909 * eV,
    2.91727512963 * eV, 3.09960482523 * eV, 3.30624514691 * eV,
    3.54240551455 * eV, 3.81489824644 * eV, 3.96749 * eV };

  // It does not reflect anything
  G4double reflectivity[entries] = {
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0. };

  tp_SiPM_optProp->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

  G4double efficiency[entries] = {
    0.036, 0.048, 0.060, 0.070, 0.090, 0.105,
    0.120, 0.145, 0.170, 0.200, 0.235, 0.275,
    0.320, 0.370, 0.420, 0.425, 0.415, 0.350,
    0.315, 0.185, 0.060 };

  tp_SiPM_optProp->AddProperty("EFFICIENCY", energies, efficiency, entries);

  G4OpticalSurface* tp_SiPM_optSurf = 
    new G4OpticalSurface(tp_SiPM_name, unified, polished, dielectric_metal);

  tp_SiPM_optSurf->SetMaterialPropertiesTable(tp_SiPM_optProp);

  new G4LogicalSkinSurface(tp_SiPM_name, tp_SiPM_logic, tp_SiPM_optSurf);

  // SiPM Sensitive detector
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
  if (!sdmgr->FindSensitiveDetector(tp_SiPM_name, false)) {
    PmtSD* tp_SiPM_sd = new PmtSD(tp_SiPM_name);
    tp_SiPM_sd->SetDetectorVolumeDepth(0);
    tp_SiPM_sd->SetMotherVolumeDepth(0);
    tp_SiPM_sd->SetTimeBinning(_SiPM_bin);

    G4SDManager::GetSDMpointer()->AddNewDetector(tp_SiPM_sd);
    tp_SiPM_case_logic->SetSensitiveDetector(tp_SiPM_sd);
  }


  /// Placing SiPM cases
  for (G4int i=0; i<_num_SiPMs; i++) {
    G4ThreeVector tp_SiPM_case_pos = _SiPM_positions[i];
    tp_SiPM_case_pos.setZ(tp_SiPM_case_posZ);
    G4int sipm_id = _first_sensor_id + i;
    new G4PVPlacement(nullptr, tp_SiPM_case_pos, tp_SiPM_case_logic, tp_SiPM_case_name,
                      _mother_logic, true, sipm_id, false);
  }


  /// Verbosity ///
  if (_verbosity) {
    G4cout << "* SiPM Z positions: " << _SiPM_iniZ
           << " to " << _SiPM_iniZ + _SiPM_case_thickness << G4endl;
  }
}


// Function that computes and stores the XY positions of SiPMs in the copper plate
void NextFlexTrackingPlane::GenerateSiPMpositions()
{
  // Maximum radius to place SiPMs
  // Lower than diameter to prevent SiPMs being partially out.
  G4double max_radius = _diameter/2. - _SiPM_size;
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
        //G4cout << posX << " " << posY << G4endl;
    }
  }

  _num_SiPMs = _SiPM_positions.size();

  if (_verbosity) {
    G4cout << "* TP num SiPM rows   : " << num_rows    << G4endl;
    G4cout << "* TP num SiPM columns: " << num_columns << G4endl;
    G4cout << "* Total num SiPMs    : " << _num_SiPMs  << G4endl;
  }
}


// Function that makes PMT holes to the solid passed by parameter
G4SubtractionSolid* NextFlexTrackingPlane::MakeSiPMholes(G4Tubs* ini_solid)
{
  // Making the holes slightly bigger than SiPMs
  G4double loose_factor = 1.5;

  G4double hole_half_sizeX = _SiPM_size * loose_factor;
  G4double hole_half_sizeY = _SiPM_size * loose_factor;
  G4double hole_half_sizeZ = ini_solid->GetDz();

  G4Box* hole_solid = new G4Box("HOLE_SOLID", hole_half_sizeX,
                                hole_half_sizeY, hole_half_sizeZ);

  // Subtracting the first PMT hole
  G4SubtractionSolid* solid_with_holes =
    new G4SubtractionSolid("SOLID_WITH_HOLES", ini_solid,
                           hole_solid, nullptr, _SiPM_positions[0]);

  // Subtracting the rest of the holes
  for (G4int i=1; i < _num_SiPMs; i++) {
    solid_with_holes =
      new G4SubtractionSolid("SOLID_WITH_HOLES", solid_with_holes,
                             hole_solid, nullptr, _SiPM_positions[i]);
  }

  return solid_with_holes;
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
