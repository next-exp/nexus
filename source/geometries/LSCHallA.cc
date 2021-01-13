// -----------------------------------------------------------------------------
// nexus | LSCHallA.cc
//
// Class which builds a simplified version of the walls of LSC HallA.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "LSCHallA.h"
#include "CylinderPointSampler2020.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4ProductionCuts.hh>
#include <G4PVPlacement.hh>
#include <G4Region.hh>
#include <G4SubtractionSolid.hh>
#include <G4Tubs.hh>
#include <G4VisAttributes.hh>


namespace nexus {

  using namespace CLHEP;

  LSCHallA::LSCHallA():
    BaseGeometry(),
    visibility_(0),
    lab_radius_(7.64 * m),
    lab_length_(39.3 * m),
    lab_wall_thickn_(3. * m),
    castle_centre_z_(14.19 * m),
    castle_centre_y_(-86 * cm)
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/LSCHallA/",
				  "Control commands of geometry HallA.");

    G4GenericMessenger::Command& rock_thickn_cmd =
    msg_->DeclareProperty("wall_thickness", lab_wall_thickn_,
                          "Thickness of surrounding rock to simulate.");
    rock_thickn_cmd.SetUnitCategory("Length");
    rock_thickn_cmd.SetParameterName("wall_thickness", false);
    rock_thickn_cmd.SetRange("wall_thickness>=0.");

    msg_->DeclareProperty("rock_vis", visibility_, "Rock Visibility");
  }

  LSCHallA::~LSCHallA()
  {
  }

  void LSCHallA::Construct()
  {
    // Build up the solid for the walls and ceiling of the lab
    // Simplified to be a cylinder.
    G4Tubs * lab_outer = new G4Tubs("lab_outer", 0,
				    lab_radius_ + lab_wall_thickn_,
				    lab_length_ / 2. + lab_wall_thickn_,
				    0, 360 * deg);
    G4Tubs * lab_inner = new G4Tubs("lab_inner", 0,
				    lab_radius_, lab_length_ / 2.,
				    0, 360 * deg);
    G4SubtractionSolid *lab_walls = new G4SubtractionSolid("lab_walls",
							   lab_outer,
							   lab_inner);

    // Overall air logical volume as mother
    G4LogicalVolume* lab_air_logic =
      new G4LogicalVolume(lab_outer, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "lab_air");
    this->SetLogicalVolume(lab_air_logic);

    // The walls
    G4LogicalVolume* rock_logic =
      new G4LogicalVolume(lab_walls, MaterialsList::Limestone(), "lab_walls");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), rock_logic,
		      "lab_walls", lab_air_logic, false, 0, false);

    if (visibility_)
      rock_logic->SetVisAttributes(nexus::DarkGrey());
    else
      rock_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Limit tracking detail in the rock
    G4Region* rock_region = new G4Region("LAB_ROCK");
    G4ProductionCuts *rock_prod_cut = new G4ProductionCuts();
    rock_prod_cut->SetProductionCut(50. * cm);
    rock_region->AddRootLogicalVolume(rock_logic);
    rock_region->SetProductionCuts(rock_prod_cut);

    // Basic sampling for inside of the walls (neutrons?)
    G4ThreeVector hall_centre(0., -castle_centre_y_, -castle_centre_z_);
    hallA_vertex_gen_ =
      new CylinderPointSampler2020(lab_radius_ - 0.1 * mm,
				   lab_radius_, (lab_length_ - 0.1 *mm) /2.,
				   0., twopi, nullptr, hall_centre);
    hallA_outer_gen_ =
      new CylinderPointSampler2020(lab_radius_ + lab_wall_thickn_ + 0.1 * mm,
				   lab_radius_ + lab_wall_thickn_ + 0.2 * mm,
				   lab_length_/2. + lab_wall_thickn_ + 0.1 * mm,
				   0, twopi, nullptr, hall_centre);
  }

  G4ThreeVector LSCHallA::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "HALLA_INNER")
      return hallA_vertex_gen_->GenerateVertex("INNER_SURFACE");
    else if (region == "HALLA_OUTER")
      return hallA_outer_gen_->GenerateVertex("INNER_SURFACE");
    // Unknown region
    else {
      G4Exception("[LSCHallA]", "GenerateVertex()", FatalException,
		  "Unknown Region!");
    }

    return vertex;
  }

  G4ThreeVector LSCHallA::ProjectToRegion(const G4String& region,
					  const G4ThreeVector& point,
					  const G4ThreeVector& dir) const
  {
    // Project along dir from point to find the first intersection
    // with region.
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "HALLA_INNER")
      return hallA_vertex_gen_->GetIntersect(point, dir);
    else if (region == "HALLA_OUTER")
      return hallA_outer_gen_->GetIntersect(point, dir);
    else {
      G4Exception("[LSCHallA]", "ProjectToRegion()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }

}
