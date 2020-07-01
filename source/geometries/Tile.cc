// ----------------------------------------------------------------------------
// nexus | Tile.cc
//
// Geometry of a basic tile.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Tile.h"
#include "PmtSD.h"
#include "ToFSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "SiPMpetFBK.h"

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

  Tile::Tile(): BaseGeometry(),
		visibility_(0),
		tile_x_(19.8 * mm),
		tile_y_(38.6 * mm),
		tile_z_(3. * mm),
		sipm_pitch_(4.7 * mm),
		n_rows_(8),
		n_columns_(4)

  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/Tile/", "Control commands of geometry.");
    msg_->DeclareProperty("visibility", visibility_, "Tile Visibility");

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

    sipm_ = new SiPMpetFBK();
  }

  Tile::~Tile()
  {
  }

  void Tile::Construct()
  {
    SetDimensions(G4ThreeVector(tile_x_, tile_y_, tile_z_));

    G4Box* tile_solid = new G4Box("TILE", tile_x_/2., tile_y_/2., tile_z_/2);

    G4Material* epoxy = MaterialsList::Epoxy();
    // G4cout << "Epoxy used with constant refraction index = " <<  refr_index_ << G4endl;
    //epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::EpoxyFixedRefr(refr_index_));

    G4LogicalVolume* tile_logic =
      new G4LogicalVolume(tile_solid, epoxy, "TILE");

    this->SetLogicalVolume(tile_logic);

    sipm_->Construct();
    G4ThreeVector sipm_dim = sipm_->GetDimensions();

    G4double offset_x = (tile_x_ - ((n_columns_ - 1) * sipm_pitch_) - sipm_dim.x())/2.;
    G4double offset_y = (tile_y_ - ((n_rows_ - 1) * sipm_pitch_) - sipm_dim.y())/2.;

    G4double air_x = tile_x_ - offset_x;
    G4double air_y = tile_y_ - offset_y;
    G4double air_z = 1.2 * mm + sipm_dim.z();

    G4Box* air_solid = new G4Box("TILE_AIR", air_x/2., air_y/2., air_z/2);

    G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    G4LogicalVolume* air_logic =
      new G4LogicalVolume(air_solid, air, "TILE_AIR");

    new G4PVPlacement(0, G4ThreeVector(0., 0., tile_z_/2. - air_z/2.),
		      air_logic, "TILE_AIR", tile_logic, false, 0, true);


    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();
    //G4double support_thickness = 1. * mm;

    G4int count = 0;
    for (int i=0; i<n_columns_; i++){
      for (int j=0; j<n_rows_; j++){
	count += 1;
	new G4PVPlacement(0, G4ThreeVector(-tile_x_/2. + offset_x + sipm_dim.x()/2. + i * sipm_pitch_,
                                           -tile_y_/2.
					   + offset_y + sipm_dim.y()/2. + j * sipm_pitch_,
					   tile_z_/2. - air_z + sipm_dim.z()/2.),
			  sipm_logic, "SiPMpetFBK", air_logic, false, count, true);
	}
      }


    G4double quartz_x = tile_x_ - offset_x;
    G4double quartz_y = tile_y_ - offset_y;
    G4double quartz_z = 0.6 * mm;

    G4Box* quartz_solid = new G4Box("TILE_WINDOW", quartz_x/2., quartz_y/2., quartz_z/2);

    G4Material* quartz = MaterialsList::FusedSilica();
    quartz->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
    G4LogicalVolume* quartz_logic =
      new G4LogicalVolume(quartz_solid, quartz, "TILE_WINDOW");

    new G4PVPlacement(0, G4ThreeVector(0., 0., air_z/2. - quartz_z/2.),
    		      quartz_logic, "TILE_WINDOW", air_logic, false, 0, true);


    // Visibilities
    if (visibility_) {
      G4VisAttributes tile_col = nexus::Lilla();
      tile_logic->SetVisAttributes(tile_col);
      G4VisAttributes quartz_col = nexus::DarkGrey();
      quartz_col.SetForceSolid(true);
      quartz_logic->SetVisAttributes(quartz_col);
      G4VisAttributes air_col = nexus::LightBlue();
      //air_col.SetForceSolid(true);
      air_logic->SetVisAttributes(air_col);

    }
    else {
      tile_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

G4ThreeVector Tile::GetDimensions() {

  return G4ThreeVector(tile_x_, tile_y_, tile_z_);

}


} // end namespace nexus
