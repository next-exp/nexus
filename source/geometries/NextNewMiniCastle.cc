// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 3 Feb 2014
//
//  Copyright (c) 2014 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextNewMiniCastle.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4UnionSolid.hh>
#include <G4VisAttributes.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <Randomize.hh>
#include <G4NistManager.hh>
#include <G4UserLimits.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  NextNewMiniCastle::NextNewMiniCastle():
    BaseGeometry(),
    // Body dimensions
    _x (1447. *mm),
    _y (1062. *mm),
    _z (1494. *mm),
    _thickness (50. *mm),
    _visibility(0)

  {
    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("mini_castle_vis", _visibility, "Mini Castle Visibility");
  }

  void NextNewMiniCastle::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  void NextNewMiniCastle::Construct()
  {
    ////// LEAD MINI CASTLE ///////////
    G4Box* x_wall_solid = new G4Box("X_WALL_SOLID", _thickness/2., _y/2., _z/2.);
    G4Box* y_wall_solid = new G4Box("Y_WALL_SOLID", _x/2., _thickness/2., _z/2.);
    G4Box* z_wall_nh_solid = new G4Box("Z_WALL_NH_SOLID", _x/2., _y/2., _thickness/2.);
    G4Box* z_opening_pmts = new G4Box("Z_OP_PMTS", 525/2.*mm, 488/2.*mm,(_thickness+1*mm)/2.);
    G4Box* z_opening_sipms = new G4Box("Z_OP_SIPMS", 725./2.*mm, 488./2.*mm,(_thickness+1*mm)/2.);
    G4SubtractionSolid* z_wall_solid_pmts =
      new G4SubtractionSolid("Z_WALL_SOLID_PMTs", z_wall_nh_solid, z_opening_pmts, 0,
			     G4ThreeVector(0., 0., 0.));
    G4SubtractionSolid* z_wall_solid_sipms =
      new G4SubtractionSolid("Z_WALL_SOLID_SiPMs", z_wall_nh_solid, z_opening_sipms, 0,
			     G4ThreeVector(0., 0., 0.));

    G4UnionSolid* castle_solid =
      new G4UnionSolid("MINI_CASTLE", y_wall_solid, x_wall_solid, 0,
		       G4ThreeVector(_x/2.-_thickness/2., -_y/2.+_thickness/2., 0.));
    castle_solid =
      new G4UnionSolid("MINI_CASTLE", castle_solid, x_wall_solid,0,
		       G4ThreeVector(-_x/2.+_thickness/2., -_y/2.+_thickness/2., 0.));

    // castle_solid =
    //   new G4UnionSolid("MINI_CASTLE", castle_solid, y_wall_solid, 0,
    // 		       G4ThreeVector(0., -_y+_thickness,0.));

    castle_solid =
      new G4UnionSolid("MINI_CASTLE", castle_solid, z_wall_solid_pmts, 0,
    		       G4ThreeVector(0., -_y/2.+_thickness/2., -_z/2.+_thickness/2.));
    castle_solid =
      new G4UnionSolid("MINI_CASTLE", castle_solid, z_wall_solid_sipms, 0,
     		       G4ThreeVector(0., -_y/2.+_thickness/2., _z/2.-_thickness/2.));

    G4LogicalVolume* castle_logic =
      new G4LogicalVolume (castle_solid,
			   G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"),"CASTLE");
    new G4PVPlacement(0, G4ThreeVector(0., _y/2.-_thickness, 0.), castle_logic,
		      "MINI_CASTLE", _mother_logic, false, 0, true);

    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes lead_col = nexus::DarkGrey();
      lead_col.SetForceSolid(true);
      castle_logic->SetVisAttributes(lead_col);
    } else {
      castle_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    // VERTEX GENERATORS   //////////
    _mini_castle_box_gen =
      new BoxPointSampler(_x-2.*_thickness, _y-2.*_thickness, _z-2.*_thickness,
			  _thickness, G4ThreeVector(0.,_thickness,0.), 0);
    _mini_castle_external_surf_gen =
      new BoxPointSampler(_x-0.5*mm, _y-0.5*mm, _z-0.5*mm, 0 * mm, G4ThreeVector(0., _thickness, 0.), 0);

    // Calculating some probs
    G4double castle_vol = castle_solid->GetCubicVolume();
    G4double inner_vol= (_x-2*_thickness)*(_y-2*_thickness)*(_z-2*_thickness);
    std::cout<<"MINI CASTLE VOLUME:\t"<<castle_vol<<"\t INNER VOLUME:\t"<<inner_vol<<std::endl;

  }

  NextNewMiniCastle::~NextNewMiniCastle()
  {
    delete _mini_castle_box_gen;
    delete _mini_castle_external_surf_gen;
  }

  G4ThreeVector NextNewMiniCastle::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "MINI_CASTLE") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _mini_castle_box_gen->GenerateVertex("WHOLE_VOL");
	// To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "MINI_CASTLE");
    }
    else if (region == "RN_MINI_CASTLE") {
	G4VPhysicalVolume *VertexVolume;
	do {
	  vertex = _mini_castle_external_surf_gen->GenerateVertex("WHOLE_SURF");
	  // To check its volume, one needs to rotate and shift the vertex
	  // because the check is done using global coordinates
	  G4ThreeVector glob_vtx(vertex);
	  // First rotate, then shift
	  glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	  glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	  VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	} while (VertexVolume->GetName() != "MINI_CASTLE");
      }
    else {
      G4Exception("[NextNewMiniCastle]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }

} //end namespace nexus
