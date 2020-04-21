// -----------------------------------------------------------------------------
//  nexus | Next100SiPMBoard.cc
//
//  * Author: <justo.martin-albo@ific.uv.es>
//  * Creation date: 7 January 2020
// -----------------------------------------------------------------------------

#include "Next100SiPMBoard.h"

#include "MaterialsList.h"
#include "GenericPhotosensor.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>

using namespace nexus;


Next100SiPMBoard::Next100SiPMBoard():
  BaseGeometry     (),
  size_            (123.80 * mm),
  board_thickness_ (  1.   * mm),
  pitch_           ( 15.60 * mm),
  margin_          (  7.30 * mm),
  mask_thickness_  (  5.   * mm),
  mpv_             (nullptr),
  vtxgen_          (nullptr),
  sipm_            (new GenericPhotosensor("SiPM", 1.3 * mm))
{
}


Next100SiPMBoard::~Next100SiPMBoard()
{
  delete vtxgen_;
  delete sipm_;
}


void Next100SiPMBoard::Construct()
{
  if (!mpv_)
    G4Exception("[Next100SiPMBoard]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  G4Material* mother_gas = mpv_->GetLogicalVolume()->GetMaterial();


  /// KAPTON BOARD ///  This is the volume that contains everything
  G4String board_name = "SIPM_BOARD";

  G4Material* kapton  = 
    G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");

  G4Box* board_solid_vol = new G4Box(board_name, size_/2., size_/2.,
                                     (board_thickness_ + mask_thickness_)/2.);

  G4LogicalVolume* board_logic_vol =
    new G4LogicalVolume(board_solid_vol, kapton, board_name);

  BaseGeometry::SetLogicalVolume(board_logic_vol);


  /// TEFLON MASK ///  
  G4String mask_name = "SIPM_BOARD_MASK";
  G4double mask_posZ = board_thickness_/2.;

  G4Material* teflon =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

  G4Box* mask_solid_vol =
    new G4Box(mask_name, size_/2., size_/2., mask_thickness_/2.);

  G4LogicalVolume* mask_logic_vol =
    new G4LogicalVolume(mask_solid_vol, teflon, mask_name);

  // Adding the teflon optical surface
  G4OpticalSurface* teflon_optSurf = 
    new G4OpticalSurface(mask_name + "_optSurf", unified, ground, dielectric_metal);
  teflon_optSurf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
  new G4LogicalSkinSurface(mask_name + "_optSurf", mask_logic_vol, teflon_optSurf);


  /// TEFLON MASK WLS ///  
  G4String mask_wls_name = "SIPM_BOARD_MASK_WLS";
  G4double wls_thickness = 1. * um;
  G4double mask_wls_posZ = mask_thickness_/2. - wls_thickness/2.;

  G4Material* tpb = MaterialsList::TPB();
  tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());

  G4Box* mask_wls_solid_vol =
    new G4Box(mask_wls_name, size_/2., size_/2., wls_thickness/2.);

  G4LogicalVolume* mask_wls_logic_vol =
    new G4LogicalVolume(mask_wls_solid_vol, tpb, mask_wls_name);

  G4VPhysicalVolume* mask_wls_phys_vol =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., mask_wls_posZ),
                      mask_wls_logic_vol, mask_wls_name, mask_logic_vol,
                      false, 0, false);

  // Adding the WLS optical surface
  G4OpticalSurface* mask_wls_optSurf =
    new G4OpticalSurface(mask_wls_name + "_optSurf", glisur, ground,
                         dielectric_dielectric, .01);
  new G4LogicalBorderSurface(mask_wls_name + "_optSurf", mask_wls_phys_vol,
                             mpv_, mask_wls_optSurf);
  new G4LogicalBorderSurface(mask_wls_name + "_optSurf", mpv_,
                             mask_wls_phys_vol, mask_wls_optSurf);


  /// Adding the SiPMs ////////////////////////////////////////////////////

  /// the SiPM ///  
  // We use for now the generic photosensor until the exact characteristics
  // of the new Hamamatsu SiPMs are known.

  G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
  G4double energy[]       = {0.2 * eV, 11.5 * eV};
  G4double reflectivity[] = {0.0     ,  0.0};
  G4double efficiency[]   = {1.0     ,  1.0};
  photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
  photosensor_mpt->AddProperty("EFFICIENCY",   energy, efficiency,   2);
  sipm_->SetOpticalProperties(photosensor_mpt);

  sipm_->SetWithWLSCoating(true);
  //sipm_->SetWindowRefractiveIndex(mother_gas->GetMaterialPropertiesTable()
  //                                ->GetProperty("RINDEX"));

  sipm_->SetSensorDepth(2);
  sipm_->SetMotherDepth(4);
  sipm_->SetNamingOrder(1000);
  sipm_->Construct();


  /// MASK WLS HOLE ///
  G4String mask_wls_hole_name   = "SIPM_BOARD_MASK_WLS_HOLE";
  G4double mask_wls_hole_diam   = 7. * mm;
  G4double mask_wls_hole_length = wls_thickness;

  G4Tubs* mask_wls_hole_solid_vol =
    new G4Tubs(mask_wls_hole_name, 0., mask_wls_hole_diam/2.,
               mask_wls_hole_length/2., 0, twopi);

  G4LogicalVolume* mask_wls_hole_logic_vol = 
    new G4LogicalVolume(mask_wls_hole_solid_vol, mother_gas, mask_wls_hole_name);


  /// MASK HOLE ///
  G4String mask_hole_name   = "SIPM_BOARD_MASK_HOLE";
  G4double mask_hole_diam   = mask_wls_hole_diam;
  G4double mask_hole_length = mask_thickness_      - wls_thickness;
  G4double mask_hole_posZ   = - mask_thickness_/2. + mask_hole_length/2.;

  G4Tubs* mask_hole_solid_vol =
    new G4Tubs(mask_hole_name, 0., mask_hole_diam/2.,
               mask_hole_length/2., 0, twopi);

  G4LogicalVolume* mask_hole_logic_vol = 
    new G4LogicalVolume(mask_hole_solid_vol, mother_gas, mask_hole_name);


  /// Placing the SiPM into the MASK HOLE ///
  G4double sipm_posZ = - mask_hole_length/2. + sipm_->GetThickness()/2.;

  new G4PVPlacement(0, G4ThreeVector(0., 0., sipm_posZ), sipm_->GetLogicalVolume(),
                    sipm_->GetName(), mask_hole_logic_vol, false, 0, false);


  /// Replicating the MASK & MASK_WLS holes ///
  G4double sipm_posZ_inBoard = board_thickness_ + sipm_->GetThickness()/2.;
  G4int counter = 0;
  for (unsigned int i=0; i<8; i++) {
    G4double xpos = -size_/2. + margin_ + i * pitch_;

    for (unsigned int j=0; j<8; j++) {
      G4double ypos = -size_/2. + margin_ + j * pitch_;

      G4ThreeVector sipm_position(xpos, ypos, sipm_posZ_inBoard);
      sipm_positions_.push_back(sipm_position);

      G4ThreeVector mask_hole_pos(xpos, ypos, mask_hole_posZ);
      new G4PVPlacement(nullptr, mask_hole_pos, mask_hole_logic_vol, mask_hole_name,
                        mask_logic_vol, true, counter, false);

      G4ThreeVector mask_wls_hole_pos(xpos, ypos, 0);
      new G4PVPlacement(nullptr, mask_wls_hole_pos, mask_wls_hole_logic_vol,
                        mask_wls_hole_name, mask_wls_logic_vol, true, counter, false);

      //G4cout << "* SiPM " << counter << " position: " 
      //       << sipm_position << G4endl;

      counter++;
    }
  }


  /// Placing the MASK with HOLES and SiPMs ///
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., mask_posZ), mask_logic_vol,
                    mask_name, board_logic_vol, false, 0, false);


  /// VERTEX GENERATOR ///
  vtxgen_ = new BoxPointSampler(size_, size_, board_thickness_, 0.,
                                G4ThreeVector(0., 0., -mask_thickness_/2.));

  /// VISIBILITIES ///
  G4VisAttributes light_blue = LightBlue();
  board_logic_vol ->SetVisAttributes(light_blue);
  mask_logic_vol  ->SetVisAttributes(light_blue);

  mask_wls_logic_vol     ->SetVisAttributes(G4VisAttributes::Invisible);
  mask_hole_logic_vol    ->SetVisAttributes(G4VisAttributes::Invisible);
  mask_wls_hole_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
}



G4ThreeVector Next100SiPMBoard::GenerateVertex(const G4String&) const
{
  // Only one generation region available at the moment
  return vtxgen_->GenerateVertex("INSIDE");
}
