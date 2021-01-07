// ----------------------------------------------------------------------------
//  nexus | BlackBoxSingle.cc
//  SensL SiPM in a black box.
//  Author:  Miryam Martínez Vara
// ----------------------------------------------------------------------------

#include "BlackBoxSingle.h"
#include "SiPMSensl.h"

#include "BaseGeometry.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4GenericMessenger.hh>
#include <G4Orb.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <Randomize.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4OpticalSurface.hh>
#include <G4RotationMatrix.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

   BlackBoxSingle::BlackBoxSingle():
    _world_z (2. * m),
    _world_xy (1. *m),
    _visibility(0)
  {
    _msg = new G4GenericMessenger(this, "/Geometry/BlackBoxSingle/",
				  "Control commands of BlackBoxSingle.");
    _msg->DeclareProperty("visibility", _visibility, "Giant detectors visibility");

    G4GenericMessenger::Command&  specific_vertex_X_cmd =
      _msg->DeclareProperty("specific_vertex_X", _specific_vertex_X,
                            "If region is AD_HOC, x coord of primary particles");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
      _msg->DeclareProperty("specific_vertex_Y", _specific_vertex_Y,
                            "If region is AD_HOC, y coord of primary particles");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
      _msg->DeclareProperty("specific_vertex_Z", _specific_vertex_Z,
                            "If region is AD_HOC, z coord of primary particles");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  sipm_z_pos_cmd =
      _msg->DeclareProperty("sipm_z_pos", _sipm_z_pos,
                            "Distance between dice and photon source");
    sipm_z_pos_cmd.SetParameterName("sipm_z_pos", true);
    sipm_z_pos_cmd.SetUnitCategory("Length");

    sipm_ = new SiPMSensl;
  }

  BlackBoxSingle::~BlackBoxSingle()
  {
    delete _msg;
    delete sipm_;
  }

  void BlackBoxSingle::Construct()
  {
  /// Constructing the SiPM ///
  sipm_->SetSensorDepth(3);
  sipm_->SetMotherDepth(1);
  sipm_->SetNamingOrder(1000);
  sipm_->Construct();

  // WORLD /////////////////////////////////////////////////

  G4String world_name = "WORLD";

  G4Material* world_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  world_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

  G4Box* world_solid_vol =
    new G4Box(world_name, _world_xy/2., _world_xy/2., _world_z/2.);

  G4LogicalVolume* world_logic_vol =
    new G4LogicalVolume(world_solid_vol, world_mat, world_name);
  world_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
  BaseGeometry::SetLogicalVolume(world_logic_vol);

  // SiPM //////////////////////////////////////////////

  G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();
  ////SiPM placement
  sipm_x_pos_ = 0 * cm;
  sipm_y_pos_ = 0 * cm;
  G4ThreeVector post(sipm_x_pos_,sipm_y_pos_,_sipm_z_pos);
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot -> rotateY(180*deg);

  new G4PVPlacement(rot, post, sipm_logic,
	            "SiPM", world_logic_vol, false, 0, false);
  }

    G4ThreeVector BlackBoxSingle::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // WORLD
    if (region == "WORLD") {
      vertex = G4ThreeVector(0.,0.,0.*mm);
    }
    else if (region == "AD_HOC") {
      // AD_HOC does not need to be shifted because it is passed by the user
      vertex = G4ThreeVector(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
      return vertex;
    }
    else {
      G4Exception("[BlackBoxSingle]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    G4ThreeVector displacement = G4ThreeVector(0., 0., 0.);
    vertex = vertex + displacement;
    return vertex;
  }

} // end namespace nexus