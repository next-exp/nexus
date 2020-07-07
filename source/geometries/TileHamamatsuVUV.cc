// ----------------------------------------------------------------------------
// nexus | TileHamamatsuVUV.cc
//
// Hamamatsu VUV sensor tile geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "TileHamamatsuVUV.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "SiPMpetVUV_new.h"
#include "IonizationSD.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

#include <CLHEP/Units/SystemOfUnits.h>


namespace nexus {

  using namespace CLHEP;

  TileHamamatsuVUV::TileHamamatsuVUV(): BaseGeometry(),
		visibility_(0),
        reflectivity_(0.),
		tile_x_(30.9 * mm),
		tile_y_(30.7 * mm),
		tile_z_(3. * mm),
		sipm_pitch_(7.5 * mm),
		n_rows_(4),
		n_columns_(4),
        lxe_thick_(0.6 * mm),
        quartz_thick_(0.6 * mm),
        quartz_transparency_(0.9)

  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/Tile/", "Control commands of geometry.");
    msg_->DeclareProperty("visibility", visibility_, "Tile Visibility");
    msg_->DeclareProperty("reflectivity", reflectivity_, "Reflectivity for FR4");

    G4GenericMessenger::Command& size_x_cmd =
      msg_->DeclareProperty("size_x", tile_x_, "Size of tile X");
    size_x_cmd.SetUnitCategory("Length");
    size_x_cmd.SetParameterName("size_x", false);
    size_x_cmd.SetRange("size_x>0.");

    G4GenericMessenger::Command& size_y_cmd =
      msg_->DeclareProperty("size_y", tile_y_, "Size of tile Y");
    size_y_cmd.SetUnitCategory("Length");
    size_y_cmd.SetParameterName("size_y", false);
    size_y_cmd.SetRange("size_y>0.");

    G4GenericMessenger::Command& size_z_cmd =
      msg_->DeclareProperty("size_z", tile_z_, "Size of tile Z");
    size_z_cmd.SetUnitCategory("Length");
    size_z_cmd.SetParameterName("size_z", false);
    size_z_cmd.SetRange("size_z>0.");

    G4GenericMessenger::Command& pitch_cmd =
      msg_->DeclareProperty("pitch", sipm_pitch_, "Pitch of SiPMs");
    pitch_cmd.SetUnitCategory("Length");
    pitch_cmd.SetParameterName("pitch", false);
    pitch_cmd.SetRange("pitch>0.");

    msg_->DeclareProperty("rows", n_rows_, "Number of rows");
    msg_->DeclareProperty("columns", n_columns_, "Number of columns");

    G4GenericMessenger::Command& lxe_thick_cmd =
      msg_->DeclareProperty("lxe_thick", lxe_thick_, "Thickness of volume of LXe");
    lxe_thick_cmd.SetUnitCategory("Length");
    lxe_thick_cmd.SetParameterName("lxe_thick", false);
    lxe_thick_cmd.SetRange("lxe_thick>0.");

    G4GenericMessenger::Command& quartz_thick_cmd =
      msg_->DeclareProperty("quartz_thick", quartz_thick_, "Thickness of volume of quartz");
    quartz_thick_cmd.SetUnitCategory("Length");
    quartz_thick_cmd.SetParameterName("quartz_thick", false);
    quartz_thick_cmd.SetRange("quartz_thick>0.");

    G4GenericMessenger::Command& quartz_transparency_cmd =
      msg_->DeclareProperty("quartz_transparency", quartz_transparency_,
                "Set the transparency of the quartz layer");
    quartz_transparency_cmd.SetParameterName("quartz_transparency", false);
    quartz_transparency_cmd.SetRange("quartz_transparency>0 && quartz_transparency<1");


    sipm_ = new SiPMpetVUV_new();
  }

  TileHamamatsuVUV::~TileHamamatsuVUV()
  {
  }  

  void TileHamamatsuVUV::Construct()
  {
    SetDimensions(G4ThreeVector(tile_x_, tile_y_, tile_z_));   

    G4Box* tile_solid = new G4Box("TILE_PLASTIC", tile_x_/2., tile_y_/2., tile_z_/2);

    G4Material* fr4 = MaterialsList::FR4();
    G4LogicalVolume* tile_logic =
      new G4LogicalVolume(tile_solid, fr4, "TILE_PLASTIC");

    this->SetLogicalVolume(tile_logic);

    // OPTICAL SURFACE FOR REFLECTION
    G4OpticalSurface* fr4_opsurf =
      new G4OpticalSurface("FR4_OPSURF", unified, polished, dielectric_metal);
    fr4_opsurf->SetMaterialPropertiesTable(OpticalMaterialProperties::ReflectantSurface(reflectivity_));

    new G4LogicalSkinSurface("FR4_OPSURF", tile_logic, fr4_opsurf);


    G4int sensor_depth = 1;
    G4int mother_depth = 2;
    sipm_->SetSensorDepth(sensor_depth);
    sipm_->SetMotherDepth(mother_depth);

    sipm_->Construct();
    G4ThreeVector sipm_dim = sipm_->GetDimensions();

    G4double offset_x = (tile_x_ - ((n_columns_ - 1) * sipm_pitch_) - sipm_dim.x())/2.;
    G4double offset_y = (tile_y_ - ((n_rows_ - 1) * sipm_pitch_) - sipm_dim.y())/2.;

    // LXe
    G4double lxe_x = tile_x_ - offset_x;
    G4double lxe_y = tile_y_ - offset_y;
    G4Box* lxe_solid = new G4Box("TILE_LXE", lxe_x/2., lxe_y/2., (lxe_thick_+quartz_thick_)/2.);

    G4Material* LXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
      LXe->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    G4LogicalVolume* lxe_logic =
      new G4LogicalVolume(lxe_solid, LXe, "TILE_LXE");

    G4double zpos_lxe = tile_z_/2. - (quartz_thick_+lxe_thick_)/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., zpos_lxe), lxe_logic,
                      "TILE_LXE", tile_logic, false, 0, true);

    // Layer of quartz defined inside the LXe
    G4double quartz_x = tile_x_ - offset_x;
    G4double quartz_y = tile_y_ - offset_y;

    G4Box* quartz_solid = new G4Box("TILE_QUARTZ_WINDOW", quartz_x/2., quartz_y/2., quartz_thick_/2);

    G4Material* quartz = MaterialsList::FusedSilica();
    quartz->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGenericMaterial(quartz_transparency_, quartz_thick_));

    G4LogicalVolume* quartz_logic =
    new G4LogicalVolume(quartz_solid, quartz, "TILE_QUARTZ_WINDOW");

    new G4PVPlacement(0, G4ThreeVector(0., 0., -quartz_thick_/2.), quartz_logic,
                      "TILE_QUARTZ_WINDOW", lxe_logic, false, 0, true);

    // The real LXe region as active
    G4Box* active_solid =
    new G4Box("ACTIVE_LXE_TILE", lxe_x/2., lxe_y/2., lxe_thick_/2.);
    G4LogicalVolume* active_logic =
    new G4LogicalVolume(active_solid, LXe, "ACTIVE_LXE_TILE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., lxe_thick_/2.), active_logic, "ACTIVE_LXE_TILE", lxe_logic, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive det
    IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE_LXE_TILE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);


    // SiPMs
    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();

    G4int copy_no = 0;
    for (int i=0; i<n_columns_; i++){
      for (int j=0; j<n_rows_; j++){
	copy_no += 1;
    G4double x_pos = -tile_x_/2. + offset_x + sipm_dim.x()/2. + i * sipm_pitch_;
    G4double y_pos =  tile_y_/2. - offset_y - sipm_dim.y()/2. - j * sipm_pitch_;
    G4double z_pos = tile_z_/2. - quartz_thick_ - lxe_thick_ - sipm_dim.z()/2.;
    G4String vol_name = "SiPMpetVUV_" + std::to_string(copy_no);
	new G4PVPlacement(0, G4ThreeVector(x_pos, y_pos, z_pos),
			  sipm_logic, vol_name, tile_logic, false, copy_no, true);
	}
      }

    // Visibilities
    if (visibility_) {
      G4VisAttributes tile_col = nexus::Lilla();
      tile_logic->SetVisAttributes(tile_col);
      G4VisAttributes quartz_col = nexus::Brown();
      //quartz_col.SetForceSolid(true);
      quartz_logic->SetVisAttributes(quartz_col);
      G4VisAttributes lxe_col = nexus::LightBlue();
      //lxe_col.SetForceSolid(true);
      lxe_logic->SetVisAttributes(lxe_col);
      G4VisAttributes active_col = nexus::White();
      //active_col.SetForceSolid(true);
      active_logic->SetVisAttributes(active_col);

    }
    else {
      tile_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

G4ThreeVector TileHamamatsuVUV::GetDimensions() {

  return G4ThreeVector(tile_x_, tile_y_, tile_z_);

}


} // end namespace nexus
