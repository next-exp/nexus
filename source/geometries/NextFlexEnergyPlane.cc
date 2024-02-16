// -----------------------------------------------------------------------------
//  nexus | NextFlexEnergyPlane.cc
//
//  NEXT-Flex Energy Plane geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextFlexEnergyPlane.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "XenonProperties.h"
#include "PmtR11410.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "CylinderPointSampler.h"
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
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UserLimits.hh>
#include <Randomize.hh>


using namespace nexus;


NextFlexEnergyPlane::NextFlexEnergyPlane():
  GeometryBase(),
  mother_logic_      (nullptr),
  verbosity_         (false),
  visibility_        (false),
  msg_               (nullptr),
  ep_with_PMTs_      (true),    // Implement PMTs arranged ala NEXT100
  ep_with_teflon_    (false),   // Implement a teflon mask to reflect light
  wls_matName_       ("TPB"),
  copper_thickness_  (12 * cm)  // Thickness of the copper plate
{

  // Messenger
  msg_ = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextFlex geometry.");

  // Parametrized dimensions
  DefineConfigurationParameters();

  // Hard-wired dimensions & components
  central_hole_diameter_ = 12. * mm;

  teflon_thickness_ = 5. * mm;
  wls_thickness_    = 1. * um;

  pmt_               = new PmtR11410();
  num_pmts_          = 60;        // It must be the number of PMTs in NEXT100
  pmt_hole_diameter_ = 84. * mm;  // It must be bigger than PMT diameter (84mm in NEXT100)

  window_thickness_      = 6.0 * mm;
  optical_pad_thickness_ = 1.0 * mm;

  // Initializing the geometry navigator (used in vertex generation)
  geom_navigator_ =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
}



NextFlexEnergyPlane::~NextFlexEnergyPlane()
{
  delete msg_;
  delete copper_gen_;
  if (ep_with_PMTs_) delete window_gen_;
}



void NextFlexEnergyPlane::DefineConfigurationParameters()
{
  // Verbosity
  msg_->DeclareProperty("ep_verbosity", verbosity_, "Verbosity");

  // Visibility
  msg_->DeclareProperty("ep_visibility", visibility_, "ENERGY_PLANE Visibility");

  // ENERGY_PLANE configuration
  msg_->DeclareProperty("ep_with_PMTs"  , ep_with_PMTs_  , "ENERGY_PLANE with PMTs");
  msg_->DeclareProperty("ep_with_teflon", ep_with_teflon_, "ENERGY_PLANE with teflon");

  // Copper dimensions
  G4GenericMessenger::Command& copper_thickness_cmd =
    msg_->DeclareProperty("ep_copper_thickness", copper_thickness_,
                          "Thickness of the EP Copper plate.");
  copper_thickness_cmd.SetParameterName("ep_copper_thickness", false);
  copper_thickness_cmd.SetUnitCategory("Length");
  copper_thickness_cmd.SetRange("ep_copper_thickness>=0.");

  // UV shifting material
  msg_->DeclareProperty("ep_wls_mat", wls_matName_,
                        "EP UV wavelength shifting material name");
}



void NextFlexEnergyPlane::ComputeDimensions()
{
  copper_iniZ_ = originZ_;
  teflon_iniZ_ = originZ_;
  pmt_iniZ_    = originZ_;

  // Shifting Copper placement in case of teflon
  if (ep_with_teflon_) copper_iniZ_ += teflon_thickness_;

  // If there are PMTs (for comparison with Next100) generate their positions
  // and position components accordingly
  if (ep_with_PMTs_) {
    GeneratePMTpositions();

    copper_iniZ_ = pmt_iniZ_ + 9. * mm;
    if (ep_with_teflon_) teflon_iniZ_ = copper_iniZ_ - teflon_thickness_;
  }

}



void NextFlexEnergyPlane::DefineMaterials()
{
  // Xenon
  xenon_gas_ = mother_logic_->GetMaterial();

  // Copper
  copper_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
  copper_mat_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  // Teflon
  teflon_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  teflon_mat_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  // Sapphire
  sapphire_mat_ = materials::Sapphire();
  sapphire_mat_->SetMaterialPropertiesTable(opticalprops::Sapphire());

  // Optical coupler
  optical_pad_mat_ = materials::OpticalSilicone();
  optical_pad_mat_->SetMaterialPropertiesTable(opticalprops::OptCoupler());


  // UV shifting material
  if (wls_matName_ == "NONE") {
    wls_mat_ = mother_logic_->GetMaterial();
  }
  else if (wls_matName_ == "TPB") {
    wls_mat_ = materials::TPB();
    wls_mat_->SetMaterialPropertiesTable(opticalprops::TPB());
  }
  else if (wls_matName_ == "TPH") {
    wls_mat_ = materials::TPH();
    wls_mat_->SetMaterialPropertiesTable(opticalprops::TPH());
  }
  else {
    G4Exception("[NextFlexEnergyPlane]", "DefineMaterials()", FatalException,
                "Unknown UV shifting material. Valid options are NONE, TPB or TPH.");
  }
}



void NextFlexEnergyPlane::Construct()
{
  // Make sure that the pointer to the mother volume is actually defined
  if (!mother_logic_)
    G4Exception("[NextFlexEnergyPlane]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  // Verbosity
  if(verbosity_) {
    G4cout << G4endl << "*** NEXT-Flex Energy Plane ..." << G4endl;
    G4cout << "* With PMTs:   " << ep_with_PMTs_   << G4endl;
    G4cout << "* With teflon: " << ep_with_teflon_ << G4endl;
  }

  // Getting volumes dimensions based on parameters.
  ComputeDimensions();

  // Define materials.
  DefineMaterials();

  // Copper
  BuildCopper();

  // Teflon
  if (ep_with_teflon_) { BuildTeflon(); }

  // PMTs
  if (ep_with_PMTs_) { BuildPMTs(); }
}



void NextFlexEnergyPlane::BuildCopper()
{
  G4String copper_name = "EP_COPPER";

  G4double copper_posZ = copper_iniZ_ + copper_thickness_/2.;
  copper_finZ_         = copper_iniZ_ + copper_thickness_;

  G4Tubs* copper_solid_no_holes =
    new G4Tubs(copper_name, 0., diameter_/2., copper_thickness_/2., 0, twopi);

  G4SubtractionSolid* copper_solid = MakeHoles(copper_solid_no_holes);

  G4LogicalVolume* copper_logic =
    new G4LogicalVolume(copper_solid, copper_mat_, copper_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., copper_posZ), copper_logic,
                    copper_name, mother_logic_, false, 0, verbosity_);

  // Vertex generator
  copper_gen_ = new CylinderPointSampler(0., diameter_/2., copper_thickness_/2.,
                                         0, twopi, nullptr,
                                         G4ThreeVector(0., 0., copper_posZ));

  // Visibility
  if (visibility_) copper_logic->SetVisAttributes(nexus::CopperBrown());
  else             copper_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Verbosity
  if (verbosity_) {
    G4cout << "* EP Copper Z positions: " << copper_iniZ_
           << " to " << copper_finZ_ << G4endl;
  }
}



void NextFlexEnergyPlane::BuildTeflon()
{
  /// The teflon ///
  G4String teflon_name = "EP_TEFLON";

  G4double teflon_posZ = teflon_iniZ_ + teflon_thickness_/2.;

  G4Tubs* teflon_solid_no_holes =
    new G4Tubs(teflon_name, 0., diameter_/2., teflon_thickness_/2., 0, twopi);

  G4SubtractionSolid* teflon_solid =MakeHoles(teflon_solid_no_holes);

  G4LogicalVolume* teflon_logic =
    new G4LogicalVolume(teflon_solid, teflon_mat_, teflon_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., teflon_posZ), teflon_logic,
                    teflon_name, mother_logic_, false, 0, verbosity_);

  // Adding the optical surface
  G4OpticalSurface* teflon_optSurf =
    new G4OpticalSurface(teflon_name, unified, ground, dielectric_metal);

  teflon_optSurf->SetMaterialPropertiesTable(opticalprops::PTFE());

  new G4LogicalSkinSurface(teflon_name, teflon_logic, teflon_optSurf);


  /// The UV wavelength Shifter in TEFLON ///
  G4String teflon_wls_name = "EP_TEFLON_WLS";

  G4double teflon_wls_posZ = - teflon_thickness_/2. + wls_thickness_/2.;

  G4Tubs* teflon_wls_solid_no_holes =
    new G4Tubs(teflon_wls_name, 0., diameter_/2., wls_thickness_/2., 0, twopi);

  G4SubtractionSolid* teflon_wls_solid = MakeHoles(teflon_wls_solid_no_holes);

  G4LogicalVolume* teflon_wls_logic =
    new G4LogicalVolume(teflon_wls_solid, wls_mat_, teflon_wls_name);

  G4VPhysicalVolume* teflon_wls_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., teflon_wls_posZ), teflon_wls_logic,
                      teflon_wls_name, teflon_logic, false, 0, verbosity_);

  // Optical surface
  G4OpticalSurface* teflon_wls_optSurf =
    new G4OpticalSurface("TEFLON_WLS_OPSURF", glisur, ground,
                         dielectric_dielectric, .01);

  new G4LogicalBorderSurface("TEFLON_WLS_GAS_OPSURF", teflon_wls_phys,
                             neigh_gas_phys_, teflon_wls_optSurf);
  new G4LogicalBorderSurface("GAS_TEFLON_WLS_OPSURF", neigh_gas_phys_,
                             teflon_wls_phys, teflon_wls_optSurf);


  /// Visibility ///
  if (visibility_) {
    G4VisAttributes light_blue_col = nexus::LightBlue();
    light_blue_col.SetForceSolid(true);
    teflon_logic->SetVisAttributes(light_blue_col);
  }
  else teflon_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  teflon_wls_logic->SetVisAttributes(G4VisAttributes::GetInvisible());


  /// Verbosity ///
  if (verbosity_) {
    G4cout << "* EP Teflon Z positions: " << teflon_iniZ_
           << " to " << teflon_iniZ_ + teflon_thickness_ << G4endl;
  }
}



void NextFlexEnergyPlane::BuildPMTs()
{
  // This PMT arrangement replicates the one of NEXT100, so first of all
  // let's check that sensor diameter is consistent with NEXT100 diameter
  if (diameter_ > 1000. * mm) {
    G4Exception("[NextFlexEnergyPlane]", "BuildPMTs()", JustWarning,
                "Building PMTs ala NEXT100 with detector diameter much bigger");
  }
  else if (diameter_ < 984. * mm) {
    G4Exception("[NextFlexEnergyPlane]", "BuildPMTs()", FatalException,
                "Building PMTs ala NEXT100 with detector diameter much smaller");
  }


  // First we define a volume ("PMT_HOLE") to contain all PMT stuff,
  // and this volume will be the one replicated.

  /// The encapsulating volume ///
  G4String pmt_hole_name = "PMT_HOLE";

  // It must be at least the copper plate thickness
  // and enough to accomodate the overall PMTs stuff
  G4double pmt_hole_length = copper_thickness_ + 50 * mm;

  G4Tubs* pmt_hole_solid =
    new G4Tubs(pmt_hole_name, 0., pmt_hole_diameter_/2., pmt_hole_length/2., 0, twopi);

  G4LogicalVolume* pmt_hole_logic =
    new G4LogicalVolume(pmt_hole_solid, xenon_gas_, pmt_hole_name);


  /// Sapphire window ///
  G4String window_name = "EP_WINDOW";

  G4double window_posz = - pmt_hole_length/2. + window_thickness_/2.;

  G4Tubs* window_solid =
    new G4Tubs(window_name, 0., pmt_hole_diameter_/2., window_thickness_/2., 0, twopi);

  G4LogicalVolume* window_logic =
    new G4LogicalVolume(window_solid, sapphire_mat_, window_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., window_posz), window_logic,
                    window_name, pmt_hole_logic, false, 0, verbosity_);

  // Vertex generator
  window_gen_ =
    new CylinderPointSampler(0., pmt_hole_diameter_/2., window_thickness_/2.,
                             0., twopi, nullptr,
                             G4ThreeVector(0., 0., window_thickness_/2.));


  /// TPB coating on windows ///
  G4String window_wls_name = "EP_WINDOW_WLS";

  G4double window_wls_posz = - window_thickness_/2. + wls_thickness_/2.;

  G4Tubs* window_wls_solid =
    new G4Tubs(window_wls_name, 0., pmt_hole_diameter_/2., wls_thickness_/2., 0, twopi);

  G4LogicalVolume* window_wls_logic =
    new G4LogicalVolume(window_wls_solid, wls_mat_, window_wls_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., window_wls_posz), window_wls_logic,
                    window_wls_name, window_logic, false, 0, verbosity_);

  // Optical surface
  G4OpticalSurface* window_wls_optSurf =
    new G4OpticalSurface("EP_WINDOW_WLS_OPSURF", glisur, ground, dielectric_dielectric, .01);

  new G4LogicalSkinSurface("EP_WINDOW_WLS_OPSURF", window_wls_logic, window_wls_optSurf);


  /// Optical pad ///
  G4String optical_pad_name = "OPTICAL_PAD";

  G4double optical_posz = - pmt_hole_length/2. + window_thickness_
                          + optical_pad_thickness_/2.;

  G4Tubs* optical_pad_solid =
    new G4Tubs(optical_pad_name, 0., pmt_hole_diameter_/2., optical_pad_thickness_/2., 0, twopi);

  G4LogicalVolume* optical_pad_logic =
    new G4LogicalVolume(optical_pad_solid, optical_pad_mat_, optical_pad_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., optical_posz), optical_pad_logic,
                    optical_pad_name, pmt_hole_logic, false, 0, verbosity_);


  /// PMT ///
  G4String pmt_name = "PMT";

  pmt_->SetSensorDepth(3);
  pmt_->Construct();

  G4double pmt_posz = - pmt_hole_length/2.     + window_thickness_
                      + optical_pad_thickness_ + pmt_->GetRelPosition().z();

  G4LogicalVolume* pmt_logic = pmt_->GetLogicalVolume();

  G4RotationMatrix* pmt_rot = new G4RotationMatrix();
  pmt_rot->rotateY(pi);
  new G4PVPlacement(pmt_rot, G4ThreeVector(0., 0., pmt_posz), pmt_logic,
                    pmt_name, pmt_hole_logic, false, 0, verbosity_);


  /// Placing the encapsulating volumes ///
  G4double hole_posZ = pmt_iniZ_ + pmt_hole_length/2.;
  G4ThreeVector pmt_hole_pos;
  for (int pmt_id=0; pmt_id < num_pmts_; pmt_id++) {
    pmt_hole_pos = pmt_positions_[pmt_id];
    pmt_hole_pos.setZ(hole_posZ);
    new G4PVPlacement(nullptr, pmt_hole_pos, pmt_hole_logic, pmt_hole_name,
                      mother_logic_, false, first_sensor_id_ + pmt_id, false);
  }


  /// Visibility ///
  if (visibility_) {
    G4VisAttributes blue_col = nexus::Lilla();
    blue_col.SetForceSolid(true);
    window_logic->SetVisAttributes(blue_col);
  }
  else window_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  pmt_hole_logic   ->SetVisAttributes(G4VisAttributes::GetInvisible());
  window_wls_logic ->SetVisAttributes(G4VisAttributes::GetInvisible());
  optical_pad_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
}



// Function that computes and stores the XY positions of PMTs in the copper plate
void NextFlexEnergyPlane::GeneratePMTpositions()
{
  G4int num_conc_circles = 4;
  G4int num_inner_pmts   = 6;
  G4double x_pitch       = 125 * mm;
  G4double y_pitch       = 108.3 * mm;

  G4ThreeVector position(0.,0.,0.);

  for (G4int circle=1; circle<=num_conc_circles; circle++) {
    G4double radius = circle * x_pitch;
    G4double step   = 360.0 / num_inner_pmts;
    for (G4int place=0; place < num_inner_pmts; place++) {
      G4double angle = place * step;
      position.setX(radius * cos(angle*deg));
      position.setY(radius * sin(angle*deg));
      position.setZ(pmt_iniZ_);
      pmt_positions_.push_back(position);
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
        position.setZ(pmt_iniZ_);
        pmt_positions_.push_back(position);
      }
    }
  }

  // Checking
  if ((G4int) pmt_positions_.size() != num_pmts_) {
    G4cout << "\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
    G4cout << "Num. PMTs = " << num_pmts_ << ", Num. positions = "
           << pmt_positions_.size() << G4endl;
    exit(0);
  }

  if (verbosity_) {
    G4cout << "* Total num PMTs: " << pmt_positions_.size() << G4endl;
    for (int pmt_id=0; pmt_id < num_pmts_; pmt_id++)
      G4cout << "* PMT " << pmt_id << " position: " << pmt_positions_[pmt_id] << G4endl;
  }
}



// Function that makes holes (gas & PMTs) to the passed solid
G4SubtractionSolid* NextFlexEnergyPlane::MakeHoles(G4Tubs*  ini_solid)
{

  // Making the central gas hole
  G4Tubs* central_hole_solid = new G4Tubs("CENTRAL_HOLE", 0., central_hole_diameter_/2.,
                                          ini_solid->GetZHalfLength() + 1.*cm, 0., twopi);

  G4SubtractionSolid* solid_with_holes =
    new G4SubtractionSolid(ini_solid->GetName(), ini_solid, central_hole_solid,
                           0, G4ThreeVector(0.,0.,0.));

  // If there are PMTs, make corresponding holes
  if (ep_with_PMTs_) {

    // Making the pmt hole
    G4Tubs* pmt_hole_solid = new G4Tubs("PMT_HOLE", 0., pmt_hole_diameter_/2.,
                                        ini_solid->GetZHalfLength() + 1.*cm, 0., twopi);

    // Substracting the holes
    for (G4int i=0; i < num_pmts_; i++) {
      G4ThreeVector pmt_hole_pos = pmt_positions_[i];
      pmt_hole_pos.setZ(0.);
      solid_with_holes = new G4SubtractionSolid(ini_solid->GetName(), solid_with_holes,
                                                pmt_hole_solid, 0, pmt_hole_pos);
    }
  }

  return solid_with_holes;
}



G4ThreeVector NextFlexEnergyPlane::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "EP_COPPER") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex       = copper_gen_->GenerateVertex(VOLUME);
      VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(vertex, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else if (region == "EP_WINDOWS") {
    if (ep_with_PMTs_) {
      vertex = window_gen_->GenerateVertex(VOLUME);
      // XY placement
      G4double rand = num_pmts_ * G4UniformRand();
      G4ThreeVector window_pos = pmt_positions_[int(rand)];
      vertex += window_pos;
      }
    else
      G4Exception("[NextFlexEnergyPlane]", "GenerateVertex()", FatalException,
      "Trying to generate Vertices in NON-existing EP Windows");
  }

  else {
    G4Exception("[NextFlexEnergyPlane]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }

  return vertex;
}
