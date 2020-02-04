// -----------------------------------------------------------------------------
//  nexus | NextFlexEnergyPlane.cc
//
//  * Info:         : NEXT-Flex Energy Plane geometry for performance studies.
//  * Author        : <jmunoz@ific.uv.es>
//  * Creation date : January 2020
// -----------------------------------------------------------------------------

#include "NextFlexEnergyPlane.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "XenonGasProperties.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "CylinderPointSampler2020.h"
#include "Visibilities.h"

#include <G4UnitsTable.hh>
#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
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


NextFlexEnergyPlane::NextFlexEnergyPlane():
  BaseGeometry(),
  _verbosity         (false),
  _visibility        (false),
  _msg               (nullptr),
  _ep_with_PMTs      (true),    // Implement PMTs arranged ala NEXT100
  _ep_with_teflon    (false),   // Implement a teflon mask to reflect light 
  _wls_matName       ("TPB"),
  _copper_thickness  (12 * cm)  // Thickness of the copper plate
{

  // Messenger
  _msg = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextParam geometry.");

  // Parametrized dimensions
  DefineConfigurationParameters();

  // Hard-wired dimensions & components
  _teflon_thickness = 5. * mm;
  _wls_thickness    = 1. * um;       // XXXXXXXX To be checked

  _pmt                = new PmtR11410();
  _num_pmts           = 60;          // It must be the number of PMTs in NEXT100
  _pmt_hole_diameter  = 80. * mm;    // It must be bigger than PMT diameter

  _window_thickness      = 6.0 * mm;
  _optical_pad_thickness = 1.0 * mm;

  // Initializing the geometry navigator (used in vertex generation)
  _geom_navigator =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
}


NextFlexEnergyPlane::~NextFlexEnergyPlane()
{
  delete _msg;
  delete _copper_gen;
  delete _window_gen;
}


void NextFlexEnergyPlane::DefineConfigurationParameters()
{
  // Verbosity
  _msg->DeclareProperty("ep_verbosity", _verbosity, "Verbosity");

  // Visibility
  _msg->DeclareProperty("ep_visibility", _visibility, "ENERGY_PLANE Visibility");

  // ENERGY_PLANE configuration
  _msg->DeclareProperty("ep_with_PMTs"  , _ep_with_PMTs  , "ENERGY_PLANE with PMTs");
  _msg->DeclareProperty("ep_with_teflon", _ep_with_teflon, "ENERGY_PLANE with teflon");

  // Copper dimensions
  G4GenericMessenger::Command& copper_thickness_cmd =
    _msg->DeclareProperty("ep_copper_thickness", _copper_thickness,
                          "Thickness of the EP Copper plate.");
  copper_thickness_cmd.SetUnitCategory("Length");
  copper_thickness_cmd.SetParameterName("ep_copper_thickness", false);
  copper_thickness_cmd.SetRange("ep_copper_thickness>=0.");

  // UV shifting material
  _msg->DeclareProperty("ep_wls_mat", _wls_matName,
                        "EP UV wavelength shifting material name");
}


void NextFlexEnergyPlane::ComputeDimensions()
{
  _copper_iniZ = _originZ;
  _teflon_iniZ = _originZ;

  // Shifting placements in case of extra volumes
  if (_ep_with_teflon) {
    _copper_iniZ += _teflon_thickness;
  }

  // If there are PMTs generate their positions
  if (_ep_with_PMTs) GeneratePMTpositions();
}


void NextFlexEnergyPlane::DefineMaterials()
{
  // Xenon
  _xenon_gas = _mother_logic->GetMaterial();

  // Copper
  _copper_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

  // Teflon
  _teflon_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  _teflon_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  // Sapphire
  _sapphire_mat = MaterialsList::Sapphire();
  _sapphire_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::Sapphire());

  // Optical coupler
  _optical_pad_mat = MaterialsList::OpticalSilicone();
  _optical_pad_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::OptCoupler());


  // UV shifting material
  if (_wls_matName == "NONE") {
    _wls_mat = _mother_logic->GetMaterial();
  }
  else if (_wls_matName == "TPB") {
    _wls_mat = MaterialsList::TPB();
    _wls_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::NewTPB());
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


void NextFlexEnergyPlane::Construct()
{
  // Verbosity
  if(_verbosity) {
    G4cout << G4endl << "*** NEXT-Flex Energy Plane ..." << G4endl;
    G4cout << "  With PMTs:   " << _ep_with_PMTs   << G4endl;
    G4cout << "  With teflon: " << _ep_with_teflon << G4endl;
  }

  // Getting volumes dimensions based on parameters.
  ComputeDimensions();

  // Define materials.
  DefineMaterials();

  // Copper
  BuildCopper();

  // Teflon
  if (_ep_with_teflon) { BuildTeflon(); }

  // PMTs
  if (_ep_with_PMTs) { BuildPMTs(); }
}


void NextFlexEnergyPlane::BuildCopper()
{
  G4String copper_name = "EP_COPPER";

  G4double copper_posZ = _copper_iniZ + _copper_thickness/2.;

  G4Tubs* copper_solid =
    new G4Tubs(copper_name, 0., _diameter/2., _copper_thickness/2., 0, twopi);

  // If there are PMTs, make corresponding holes
  //if (_ep_with_PMTs) copper_solid = dynamic_cast<G4Tubs*> (MakePMTholes(copper_solid));
  if (_ep_with_PMTs) copper_solid = (G4Tubs*) MakePMTholes(copper_solid);

  G4LogicalVolume* copper_logic =
    new G4LogicalVolume(copper_solid, _copper_mat, copper_name);

  //G4VPhysicalVolume* copper_phys =
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., copper_posZ), copper_logic,
                    copper_name, _mother_logic, false, 0, _verbosity);

  // Visibility
  //copper_logic->SetVisAttributes(G4VisAttributes::Invisible);
  copper_logic->SetVisAttributes(nexus::CopperBrown());

  // Vertex generator
  //_copper_gen = new CylinderPointSampler2020(copper_phys);
  _copper_gen = new CylinderPointSampler2020(0., _diameter/2., _copper_thickness/2.,
                                             0, twopi, nullptr,
                                             G4ThreeVector(0., 0., copper_posZ));

  // Verbosity
  if (_verbosity) {
    G4cout << "* Copper Z positions: " << _copper_iniZ
           << " to " << _copper_iniZ + _copper_thickness << G4endl;
  }
}


void NextFlexEnergyPlane::BuildTeflon()
{
  /// The teflon ///
  G4String teflon_name = "EP_TEFLON";

  G4double teflon_posZ = _teflon_iniZ + _teflon_thickness/2.;

  G4Tubs* teflon_solid =
    new G4Tubs(teflon_name, 0., _diameter/2., _teflon_thickness/2., 0, twopi);

  // If there are PMTs, make corresponding holes
  //if (_ep_with_PMTs) teflon_solid = dynamic_cast<G4Tubs*> (MakePMTholes(teflon_solid));
  if (_ep_with_PMTs) teflon_solid = (G4Tubs*) MakePMTholes(teflon_solid);

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
  G4String teflon_wls_name = "EP_TEFLON_WLS";

  G4double teflon_wls_posZ = - _teflon_thickness/2. + _wls_thickness/2.;

  G4Tubs* teflon_wls_solid =
    new G4Tubs(teflon_wls_name, 0., _diameter/2., _wls_thickness/2., 0, twopi);

  // If there are PMTs, make corresponding holes
  //if (_ep_with_PMTs) teflon_wls_solid = dynamic_cast<G4Tubs*> (MakePMTholes(teflon_wls_solid));
  if (_ep_with_PMTs) teflon_wls_solid = (G4Tubs*) MakePMTholes(teflon_wls_solid);

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


void NextFlexEnergyPlane::BuildPMTs()
{
  // First we define a volume ("PMT_HOLE") to contain all PMT stuff,
  // and this volume will be the one replicated.

  /// The encapsulating volume ///
  G4String pmt_hole_name = "PMT_HOLE";

  // It must be at least the copper plate thickness
  // and enough to accomodate all the PMTs related stuff
  G4double pmt_hole_length = _copper_thickness + 20*mm;

  G4Tubs* pmt_hole_solid =
    new G4Tubs(pmt_hole_name, 0., _pmt_hole_diameter/2., pmt_hole_length/2., 0, twopi);

  G4LogicalVolume* pmt_hole_logic =
    new G4LogicalVolume(pmt_hole_solid, _xenon_gas, pmt_hole_name);

  // Visibility
  pmt_hole_logic->SetVisAttributes(G4VisAttributes::Invisible);


  /// Sapphire window ///
  G4String window_name = "EP_WINDOW";

  G4double window_posz = - pmt_hole_length/2. + _window_thickness/2.;

  G4Tubs* window_solid =
    new G4Tubs(window_name, 0., _pmt_hole_diameter/2., _window_thickness/2., 0, twopi);

  G4LogicalVolume* window_logic =
    new G4LogicalVolume(window_solid, _sapphire_mat, window_name);

  G4VPhysicalVolume* window_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., window_posz), window_logic,
                      window_name, pmt_hole_logic, false, 0, _verbosity);

  // Visibility
  window_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vextex generator
  _window_gen = new CylinderPointSampler2020(window_phys);


  /// TPB coating on windows ///
  G4String window_wls_name = "EP_WINDOW_WLS";

  G4double window_wls_posz = - _window_thickness/2. + _wls_thickness/2.;

  G4Tubs* window_wls_solid =
    new G4Tubs(window_wls_name, 0., _pmt_hole_diameter/2., _wls_thickness/2., 0, twopi);

  G4LogicalVolume* window_wls_logic =
    new G4LogicalVolume(window_wls_solid, _wls_mat, window_wls_name);

  //G4VPhysicalVolume* window_wls_phys =
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., window_wls_posz), window_wls_logic,
                    window_wls_name, window_logic, false, 0, _verbosity);

  // Visibility
  window_wls_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Optical surface
  G4OpticalSurface* window_wls_optSurf = 
    new G4OpticalSurface("window_wls_optSurf", glisur, ground, dielectric_dielectric, .01);

  new G4LogicalSkinSurface(window_wls_name, window_wls_logic, window_wls_optSurf);


  /// Optical pad ///
  G4String optical_pad_name = "OPTICAL_PAD";

  G4double optical_posz = - pmt_hole_length/2. + _window_thickness
                          + _optical_pad_thickness/2.;

  G4Tubs* optical_pad_solid =
    new G4Tubs(optical_pad_name, 0., _pmt_hole_diameter/2., _optical_pad_thickness/2., 0, twopi);

  G4LogicalVolume* optical_pad_logic =
    new G4LogicalVolume(optical_pad_solid, _optical_pad_mat, optical_pad_name);

  //G4VPhysicalVolume* optical_pad_phys =
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., optical_posz), optical_pad_logic,
                    optical_pad_name, pmt_hole_logic, false, 0, _verbosity);

  // Visibility
  optical_pad_logic->SetVisAttributes(G4VisAttributes::Invisible);


  /// PMT ///
  G4String pmt_name = "PMT";

  _pmt->Construct();
  
  G4double pmt_posz = - pmt_hole_length/2.  + _window_thickness
                      + _optical_pad_thickness + _pmt->GetRelPosition().z();

  G4LogicalVolume* pmt_logic = _pmt->GetLogicalVolume();

  G4RotationMatrix* pmt_rot = new G4RotationMatrix();
  pmt_rot->rotateY(pi);
  new G4PVPlacement(pmt_rot, G4ThreeVector(0., 0., pmt_posz), pmt_logic,
                    pmt_name, pmt_hole_logic, false, 0, _verbosity);


  /// Placing the encapsulating volumes ///
  _pmt_hole_posZ = _copper_iniZ + pmt_hole_length/2.;
  G4ThreeVector pmt_hole_pos;
  for (int i=0; i < _num_pmts; i++) {
    pmt_hole_pos = _pmt_positions[i];
    pmt_hole_pos.setZ(_pmt_hole_posZ);
    new G4PVPlacement(nullptr, pmt_hole_pos, pmt_hole_logic, pmt_hole_name,
                      _mother_logic, false, i, false);
  }
}


// Function that computes and stores the XY positions of PMTs in the copper plate
void NextFlexEnergyPlane::GeneratePMTpositions()
{
  G4int num_conc_circles = 4;
  G4int num_inner_pmts   = 6;
  //G4double x_pitch       = 125 * mm;
  //G4double y_pitch       = 108.3 * mm;
  // 2 mm less than NEXT100 to avoid overlap with field cage fibers
  G4double x_pitch       = 123 * mm;
  G4double y_pitch       = 106.3 * mm;

  G4ThreeVector position(0.,0.,0.);

  for (G4int circle=1; circle<=num_conc_circles; circle++) {
    G4double radius = circle * x_pitch;
    G4double step   = 360.0 / num_inner_pmts;
    for (G4int place=0; place < num_inner_pmts; place++) {
      G4double angle = place * step;
      position.setX(radius * cos(angle*deg));
      position.setY(radius * sin(angle*deg));
      _pmt_positions.push_back(position);
      //if (_verbosity) G4cout << "* PMT position: " << position << G4endl;
    }

    for (G4int i=1; i<circle; i++) {
      G4double start_x = (circle-(i * 0.5)) * x_pitch;
      G4double start_y = i * y_pitch;
      radius = std::sqrt(std::pow(start_x, 2) + std::pow(start_y, 2));
      G4double start_angle = std::atan2(start_y, start_x) / deg;
      for (G4int place=0; place<num_inner_pmts; place++) {
        G4double angle = start_angle + place * step;
        position.setX(radius * cos(angle * deg));
        position.setY(radius * sin(angle * deg));
        _pmt_positions.push_back(position);
        //if (_verbosity) G4cout << "* PMT position: " << position << G4endl;
      }
    }
  }

  // Checking
  if ((G4int) _pmt_positions.size() != _num_pmts) {
    G4cout << "\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
    G4cout << "Num. PMTs = " << _num_pmts << ", Num. positions = "
           << _pmt_positions.size() << G4endl;
    exit(0);
  }

  if (_verbosity) G4cout << "* Total num PMTs: " << _pmt_positions.size() << G4endl;
}


// Function that makes PMT holes to the solid passed by parameter
G4SubtractionSolid* NextFlexEnergyPlane::MakePMTholes(G4Tubs* ini_solid)
{
  G4double hole_halfLength = ini_solid->GetDz();

  G4Tubs* hole_solid = new G4Tubs("HOLE_SOLID", 0., _pmt_hole_diameter/2.,
                                  hole_halfLength, 0., twopi);

  // Subtracting the first PMT hole
  G4SubtractionSolid* solid_with_holes =
    new G4SubtractionSolid("SOLID_WITH_HOLES", ini_solid,
                           hole_solid, nullptr, _pmt_positions[0]);

  // Subtracting the rest of the holes
  for (G4int i=1; i < _num_pmts; i++) {
    solid_with_holes =
      new G4SubtractionSolid("SOLID_WITH_HOLES", solid_with_holes,
                             hole_solid, nullptr, _pmt_positions[i]);
  }

  return solid_with_holes;
}



G4ThreeVector NextFlexEnergyPlane::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "EP_COPPER") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex       = _copper_gen->GenerateVertex("VOLUME");
      VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else if (region == "EP_WINDOWS") {
    vertex = _window_gen->GenerateVertex("VOLUME");
    // XY placement
    G4double rand = _num_pmts * G4UniformRand();
    G4ThreeVector window_pos = _pmt_positions[int(rand)];
    vertex += window_pos;
    // Z placement
    vertex += G4ThreeVector(0., 0., _pmt_hole_posZ);
  }

  else {
    G4Exception("[NextNew]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }

  return vertex;
}
