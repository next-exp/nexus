// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 3 Feb 2014
//  
//  Copyright (c) 2014 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewCuCastle.h"
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

namespace nexus {

  using namespace CLHEP;

  NextNewCuCastle::NextNewCuCastle():
    BaseGeometry(),
    // Body dimensions
    _x (1020. *mm),
    _y (900. *mm),
    _z (1440. *mm),
    _thickness (60. *mm)    
  
  {
    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
   
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("CuCastle_vis", _visibility, "Copper Castle Visibility");  
  }
   
  void NextNewCuCastle::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  void NextNewCuCastle::Construct()
  {
    ////// COPPER CASTLE ///////////
    G4Box* x_wall_solid = new G4Box("X_WALL_SOLID", _thickness/2.,_y/2.,_z/2.);
    G4Box* y_wall_solid = new G4Box("Y_WALL_SOLID", _x/2.,_thickness/2.,_z/2.);
    G4Box* z_wall_nh_solid = new G4Box("Z_WALL_NH_SOLID", _x/2.,_y/2.+_thickness,_thickness/2.);
    G4Tubs* z_hole = new G4Tubs("Z_HOLE",0.,170./2.*mm,(_thickness+1*mm)/2.,0.,twopi);
    G4SubtractionSolid* z_wall_solid = 
      new G4SubtractionSolid("Z_WALL_SOLID",z_wall_nh_solid,z_hole,0, G4ThreeVector(0.,-_thickness/2.,0.));

    G4UnionSolid* cu_castle_solid = 
      new G4UnionSolid("CU_CASTLE",y_wall_solid,x_wall_solid,0,
		       G4ThreeVector(_x/2.-_thickness/2.,-_y/2.-_thickness/2.,0.));
    cu_castle_solid= new G4UnionSolid("CU_CASTLE",cu_castle_solid,x_wall_solid,0,
				      G4ThreeVector(-_x/2.+_thickness/2.,-_y/2.-_thickness/2.,0.));
    cu_castle_solid= new G4UnionSolid("CU_CASTLE",cu_castle_solid,y_wall_solid,0,
				      G4ThreeVector(0.,-_y-_thickness,0.));
    
    cu_castle_solid= new G4UnionSolid("CU_CASTLE",cu_castle_solid,z_wall_solid,0,
				      G4ThreeVector(0.,-_y/2.-_thickness/2.,-_z/2.-_thickness/2.));
    cu_castle_solid= new G4UnionSolid("CU_CASTLE",cu_castle_solid,z_wall_solid,0,
     				      G4ThreeVector(0.,-_y/2.-_thickness/2.,_z/2.+_thickness/1.99));

    G4LogicalVolume* cu_castle_logic =
      new G4LogicalVolume (cu_castle_solid,
			   G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),"CU_CASTLE");
    G4PVPlacement* _cu_castle_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,_y/2.+_thickness,0.), cu_castle_logic,
			"CU_CASTLE", _mother_logic, false, 0, false);
   
    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes copper_col(G4Colour(.72, .45, .20));
      //steel_col.SetForceSolid(true);
      cu_castle_logic->SetVisAttributes(copper_col);
    }
    else {
      cu_castle_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    // VERTEX GENERATORS   //////////
    _cu_box_gen = new BoxPointSampler(_x-2.*_thickness,_y,_z,_thickness,G4ThreeVector(0.,_thickness,0.),0);
       		 
    // Calculating some probs
    G4double castle_vol = cu_castle_solid->GetCubicVolume();
    G4double inner_vol= (_x-2*_thickness)*_y*_z;
    std::cout<<"COPPER CASTLE VOLUME:\t"<<castle_vol<<"\t INNER VOLUME:\t"<<inner_vol<<std::endl;

  }

  NextNewCuCastle::~NextNewCuCastle()
  {
    delete _cu_box_gen;
  }
  
  G4ThreeVector NextNewCuCastle::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "CU_CASTLE") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _cu_box_gen->GenerateVertex("WHOLE_VOL");
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "CU_CASTLE");
      
    }
    return vertex;
  }

} //end namespace nexus
