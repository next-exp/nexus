// ----------------------------------------------------------------------------
// nexus | TileFBK.cc
//
// Geometry of a FBK sensor tile.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "TileFBK.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "SiPMpetFBK_new.h"
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
#include <G4PhysicalConstants.hh>

#include <CLHEP/Units/SystemOfUnits.h>


namespace nexus {

  using namespace CLHEP;

  TileFBK::TileFBK(): BaseGeometry(),
		visibility_(0),
        reflectivity_(0.),
        tile_x_(29.0 * mm),
        tile_y_(29.0 * mm),
		tile_z_(1.6 * mm),
        sipm_pitch_(3.5 * mm),
		n_rows_(8),
		n_columns_(8)

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

    sipm_ = new SiPMpetFBK_new();
  }

  TileFBK::~TileFBK()
  {
  }

  void TileFBK::Construct()
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

    G4double offset_x = 0.95* mm;
    G4double offset_y = 0.55* mm;

    // SiPMs
    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();

    for (int i=0; i<n_rows_; i++){
      for (int j=0; j<n_columns_; j++){
        G4int copy_no = i*2*n_columns_ + j + 1;
        G4double x_pos = -tile_x_/2. + offset_x + sipm_dim.x()/2. + j * sipm_pitch_;
        G4double y_pos = tile_y_/2. - offset_y - sipm_dim.y()/2. - i * sipm_pitch_;
        G4double z_pos = tile_z_/2. - sipm_dim.z()/2.;
        G4String vol_name = "SiPMpetFBK_" + std::to_string(copy_no);
        new G4PVPlacement(0, G4ThreeVector(x_pos, y_pos, z_pos),
                  sipm_logic, vol_name, tile_logic, false, copy_no, true);
	}
      }


    // Visibilities
    if (visibility_) {
      G4VisAttributes tile_col = nexus::Lilla();
      tile_logic->SetVisAttributes(tile_col);
    }
    else {
      tile_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

G4ThreeVector TileFBK::GetDimensions() {

  return G4ThreeVector(tile_x_, tile_y_, tile_z_);

}


} // end namespace nexus
