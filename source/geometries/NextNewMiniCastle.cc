// -----------------------------------------------------------------------------
// nexus | NextNewMiniCastle.cc
//
// Smaller lead castle placed inside the main lead castle at LSC.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextNewMiniCastle.h"
#include "Visibilities.h"
#include "MaterialsList.h"
#include "BoxPointSamplerLegacy.h"

#include <G4GenericMessenger.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4UnionSolid.hh>
#include <G4VisAttributes.hh>
#include <G4Box.hh>
#include <G4NistManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextNewMiniCastle::NextNewMiniCastle():
    GeometryBase(),
    // Body dimensions
    x_ (1447.*mm),
    y_ (1150.*mm),
    z_ (1494.*mm),
    thickness_ (50.*mm),
    open_space_z_(78*mm),
    steel_thickn_(2*mm),
    pedestal_surf_y_(-560.5 * mm),
    visibility_(0)

  {
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/",
                                  "Control commands of geometry NextNew.");
    msg_->DeclareProperty("minicastle_vis", visibility_, "NEW mini castle visibility");

    /// Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  }

  void NextNewMiniCastle::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }

  void NextNewMiniCastle::Construct()
  {
    ////// LEAD MINI CASTLE ///////////
    G4Box* x_wall_solid = new G4Box("X_WALL_SOLID", thickness_/2., y_/2., z_/2.);
    G4Box* y_wall_solid_up = new G4Box("Y_WALL_SOLID_UP", x_/2., thickness_/2., z_/2.);
    G4Box* y_wall_solid_down =
      new G4Box("Y_WALL_SOLID_DOWN", (x_-2.*open_space_z_-2.*thickness_)/2., thickness_/2., z_/2.);
    G4Box* z_wall_nh_solid = new G4Box("Z_WALL_NH_SOLID", x_/2., y_/2., thickness_/2.);

    G4double x_opening_pmts = 525.*mm;
    G4double x_opening_sipms = 725.*mm;
    G4double y_opening = 488.*mm;
    G4double base_opening_dist = 300.*mm;
    G4double x_crack = 12.5*mm;
    G4Box* z_opening_pmts =
      new G4Box("Z_OP_PMTS", x_opening_pmts/2., y_opening/2.,(thickness_+1*mm)/2.);
    G4Box* z_opening_sipms =
      new G4Box("Z_OP_SIPMS", x_opening_sipms/2., y_opening/2.,(thickness_+1*mm)/2.);
    G4Box* z_crack = new G4Box("Z_CRACK", 12.5*mm/2., base_opening_dist/2., (thickness_+1.*mm)/2.);

    G4SubtractionSolid* z_wall_solid_pmts =
      new G4SubtractionSolid("Z_WALL_SOLID_PMTs", z_wall_nh_solid, z_opening_pmts, 0,
    			     G4ThreeVector(0., -y_/2. + thickness_ + base_opening_dist + y_opening/2., 0.));

    z_wall_solid_pmts =
      new G4SubtractionSolid("Z_WALL_SOLID_PMTs", z_wall_solid_pmts, z_crack, 0,
     			     G4ThreeVector(-x_opening_pmts/2. + x_crack/2., -y_/2. + thickness_ + base_opening_dist/2., 0.));

    z_wall_solid_pmts =
      new G4SubtractionSolid("Z_WALL_SOLID_PMTs", z_wall_solid_pmts, z_crack, 0,
     			     G4ThreeVector(x_opening_pmts/2. + x_crack/2., -y_/2. + thickness_ + base_opening_dist/2., 0.));

    G4SubtractionSolid* z_wall_solid_sipms =
      new G4SubtractionSolid("Z_WALL_SOLID_SiPMs", z_wall_nh_solid, z_opening_sipms, 0,
			     G4ThreeVector(0., -y_/2. + thickness_ + base_opening_dist + y_opening/2., 0.));

    z_wall_solid_sipms =
      new G4SubtractionSolid("Z_WALL_SOLID_SiPMs", z_wall_solid_sipms, z_crack, 0,
     			     G4ThreeVector(-x_opening_sipms/2. + x_crack/2., -y_/2. + thickness_ + base_opening_dist/2., 0.));

    z_wall_solid_sipms =
      new G4SubtractionSolid("Z_WALL_SOLID_SiPMs", z_wall_solid_sipms, z_crack, 0,
     			     G4ThreeVector(x_opening_sipms/2. + x_crack/2., -y_/2. + thickness_ + base_opening_dist/2., 0.));


    G4UnionSolid* castle_solid =
      new G4UnionSolid("MINI_CASTLE", y_wall_solid_up, x_wall_solid, 0,
		       G4ThreeVector(x_/2.-thickness_/2., -y_/2.+thickness_/2., 0.));
    castle_solid =
      new G4UnionSolid("MINI_CASTLE", castle_solid, x_wall_solid,0,
      		       G4ThreeVector(-x_/2.+thickness_/2., -y_/2.+thickness_/2., 0.));

    castle_solid =
      new G4UnionSolid("MINI_CASTLE", castle_solid, y_wall_solid_down, 0,
     		       G4ThreeVector(0., -y_+thickness_,0.));

    castle_solid =
      new G4UnionSolid("MINI_CASTLE", castle_solid, z_wall_solid_pmts, 0,
      		       G4ThreeVector(0., -y_/2.+thickness_/2., -z_/2.+thickness_/2.));
    castle_solid =
      new G4UnionSolid("MINI_CASTLE", castle_solid, z_wall_solid_sipms, 0,
     		       G4ThreeVector(0., -y_/2.+thickness_/2., z_/2.-thickness_/2.));

    G4Material* lead = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
    lead->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* castle_logic =
      new G4LogicalVolume (castle_solid, lead, "CASTLE");

    G4double y_pos = pedestal_surf_y_ + y_ -thickness_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., y_pos, 0.), castle_logic,
    		      "MINI_CASTLE", mother_logic_, false, 0, false);

    // STEEL SUPPORT STRUCTURE //
    G4double steel_x = x_-2*thickness_;
    G4double steel_y = y_-2*thickness_;
    G4double steel_z = z_-2*thickness_;

    G4Box* x_steel_solid =
      new G4Box("X_STEEL_SOLID", steel_thickn_/2., steel_y/2., steel_z/2.);
    G4Box* y_steel_solid_up =
      new G4Box("Y_STEEL_SOLID_UP", steel_x/2., steel_thickn_/2., steel_z/2.);
    G4Box* z_steel_nh_solid =
      new G4Box("Z_STEEL_NH_SOLID", steel_x/2., steel_y/2., steel_thickn_/2.);

    G4SubtractionSolid* z_steel_solid_pmts =
      new G4SubtractionSolid("Z_STEEL_SOLID_PMTs", z_steel_nh_solid, z_opening_pmts, 0,
    			     G4ThreeVector(0., -steel_y/2. + base_opening_dist + y_opening/2., 0.));

    G4SubtractionSolid* z_steel_solid_sipms =
      new G4SubtractionSolid("Z_STEEL_SOLID_SiPMs", z_steel_nh_solid, z_opening_sipms, 0,
			     G4ThreeVector(0., -steel_y/2. + base_opening_dist + y_opening/2., 0.));

    G4UnionSolid* steel_solid =
      new G4UnionSolid("MINI_CASTLE_STEEL", y_steel_solid_up, x_steel_solid, 0,
		       G4ThreeVector(steel_x/2.-steel_thickn_/2., -steel_y/2.+steel_thickn_/2., 0.));
    steel_solid =
      new G4UnionSolid("MINI_CASTLE_STEEL", steel_solid, x_steel_solid,0,
       		       G4ThreeVector(-steel_x/2.+steel_thickn_/2., -steel_y/2.+steel_thickn_/2., 0.));

    steel_solid =
      new G4UnionSolid("MINI_CASTLE_STEEL", steel_solid, z_steel_solid_pmts, 0,
       		       G4ThreeVector(0., -steel_y/2.+steel_thickn_/2., -steel_z/2.+steel_thickn_/2.));
    steel_solid =
      new G4UnionSolid("MINI_CASTLE_STEEL", steel_solid, z_steel_solid_sipms, 0,
      		       G4ThreeVector(0., -steel_y/2.+steel_thickn_/2., steel_z/2.-steel_thickn_/2.));

    G4Material* steel = materials::Steel();
    steel->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* steel_logic =
      new G4LogicalVolume (steel_solid, steel, "MINI_CASTLE_STEEL");

    y_pos = pedestal_surf_y_ + y_ -thickness_ - steel_thickn_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., y_pos, 0.), steel_logic,
		      "MINI_CASTLE_STEEL", mother_logic_, false, 0, false);

    // VERTEX GENERATORS   //////////
    mini_castle_box_gen_ =
      new BoxPointSamplerLegacy(x_-2.*thickness_, y_-2.*thickness_, z_-2.*thickness_,
                                thickness_, G4ThreeVector(0., pedestal_surf_y_ + y_/2., 0.), 0);

    mini_castle_external_surf_gen_ =
      new BoxPointSamplerLegacy(x_-0.5*mm, y_-0.5*mm, z_-0.5*mm, 0 * mm,
                                G4ThreeVector(0., thickness_, 0.), 0);
    steel_box_gen_ =
      new BoxPointSamplerLegacy(steel_x-2.*steel_thickn_, steel_y-2.*steel_thickn_,
                                steel_z-2.*steel_thickn_, steel_thickn_,
                                G4ThreeVector(0., pedestal_surf_y_ + y_/2., 0.), 0);


    // Calculating some probs
    //G4double castle_vol = castle_solid->GetCubicVolume();
    //G4double inner_vol= (x_-2*thickness_)*(y_-2*thickness_)*(z_-2*thickness_);
    //std::cout<<"MINI CASTLE VOLUME:\t"<<castle_vol<<"\t INNER VOLUME:\t"<<inner_vol<<std::endl;

    if (!visibility_) {
      steel_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      castle_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

  }

  NextNewMiniCastle::~NextNewMiniCastle()
  {
    delete mini_castle_box_gen_;
    delete mini_castle_external_surf_gen_;
    delete steel_box_gen_;
  }

  G4ThreeVector NextNewMiniCastle::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "MINI_CASTLE") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = mini_castle_box_gen_->GenerateVertex("WHOLE_VOL");
	// To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "MINI_CASTLE");
    }
    else if (region == "RN_MINI_CASTLE") {
	G4VPhysicalVolume *VertexVolume;
	do {
	  vertex = mini_castle_external_surf_gen_->GenerateVertex("WHOLE_SURF");
	  // To check its volume, one needs to rotate and shift the vertex
	  // because the check is done using global coordinates
	  G4ThreeVector glob_vtx(vertex);
	  // First rotate, then shift
	  glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	  glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	  VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	} while (VertexVolume->GetName() != "MINI_CASTLE");
      }
    else if (region == "MINI_CASTLE_STEEL") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = steel_box_gen_->GenerateVertex("WHOLE_VOL");
	// To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "MINI_CASTLE_STEEL");
    }
    else {
      G4Exception("[NextNewMiniCastle]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }

  void NextNewMiniCastle::SetPedestalSurfacePosition(G4double ped_surf_pos)
  {
    pedestal_surf_y_ = ped_surf_pos;
  }

} //end namespace nexus
