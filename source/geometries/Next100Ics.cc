// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <justo.martin-albo@ific.uv.es>, <jmunoz@ific.uv.es>
//  Created: 21 Nov 2011
//  
//  Copyright (c) 2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next100Ics.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>

#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4Cons.hh>
#include <G4Sphere.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Colour.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;
  
  Next100Ics::Next100Ics(const G4double nozzle_ext_diam,
			 const G4double up_nozzle_ypos,
			 const G4double central_nozzle_ypos,
			 const G4double down_nozzle_ypos,
			 const G4double bottom_nozzle_ypos):
    BaseGeometry(),

    // Body dimensions
    _body_in_rad (56.0  * cm),
    _body_length (160.0 * cm),
    _body_thickness (12.0 * cm),

    // Tracking plane dimensions  (thin version without substractions)
    _tracking_orad (65.0 * cm),        // To be checked
    _tracking_length (10.0 * cm),
  
    //KDB plugs constructed here because the copper tracking plane is divided in two parts,
    // one hosted in the Next100Trackingplane class (support) and the proper shielding hosted here.
    _plug_x (40. *mm),
    _plug_y (4. *mm), //two union conectors   
    _plug_z (6. *mm),
    // Number of Dice Boards, DB columns
    _DB_columns (11),
    _num_DBs (107),

    // Energy plane dimensions
    _energy_theta (36.5 * deg),        // This must be consistent with vessel "_endcap_theta (38.6 * deg)"
    _energy_orad (108.94 * cm),        // This must be consistent with vessel "_endcap_in_rad (108.94 * cm)"
    _energy_thickness (9. * cm),
    _energy_sph_zpos (-5.76 * cm),     // This must be consistent with vessel "_endcap_z_pos (-5.76 * cm)"
    _energy_cyl_length (13.0 * cm),

    _visibility (0)
  {

    /// Needed External variables
    _nozzle_ext_diam = nozzle_ext_diam;
    _up_nozzle_ypos = up_nozzle_ypos;
    _central_nozzle_ypos = central_nozzle_ypos;
    _down_nozzle_ypos = down_nozzle_ypos;
    _bottom_nozzle_ypos = bottom_nozzle_ypos;


    // Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");
    _msg->DeclareProperty("ics_vis", _visibility, "ICS Visibility");

  }
  
  void Next100Ics::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  
  void Next100Ics::Construct()
  {

    // Dice Boards holes positions
    GenerateDBPositions();

    // ICS SOLIDS  ///////////

    // Body
    G4Tubs* ics_body_solid = new G4Tubs("ICS_BODY", _body_in_rad, _body_in_rad + _body_thickness,
					_body_length/2., 0.*deg, 360.*deg);


    // Tracking plane
    G4Tubs* ics_tracking_solid = new G4Tubs("ICS_TRACKING_NH", 0.*cm, _tracking_orad,
    					       _tracking_length/2., 0.*deg, 360.*deg);

    // // Making DB tails holes
    // G4Box* ics_tracking_hole_solid = new G4Box("ICS_TRACKING_HOLE", _plug_x/2., _plug_y,  _tracking_length);
      
    // G4SubtractionSolid* ics_tracking_solid = new G4SubtractionSolid("ICS_TRACKING", ics_tracking_nh_solid,
    //  								    ics_tracking_hole_solid, 0,_DB_positions[0]);
    // for (int i=1; i<_num_DBs; i++) {
    //   ics_tracking_solid = new G4SubtractionSolid("ICS_TRACKING", ics_tracking_solid,
    //  						  ics_tracking_hole_solid, 0, _DB_positions[i]);
    // }
    
    // Energy plane
    G4Sphere* ics_energy_sph_nh_solid = new G4Sphere("ICS_ENERGY_SPH_NH",
						     _energy_orad - _energy_thickness,  _energy_orad,   //radius
						     0. * deg, 360. * deg,                              // phi
						     180. * deg - _energy_theta, _energy_theta);        // theta
    
    G4double hole_diam = _nozzle_ext_diam + 1.*cm;
    G4double hole_length = _energy_thickness + 50.*cm;
    G4double hole_zpos = -1. * (_body_length/2. + hole_length/2.);

    G4Tubs* nozzle_hole_solid = new G4Tubs("NOZZLE_HOLE", 0.*cm, hole_diam/2., hole_length/2., 0.*deg, 360.*deg);

    G4SubtractionSolid* ics_energy_sph_solid = 
      new G4SubtractionSolid("ICS_ENERGY_SPH", ics_energy_sph_nh_solid,
			     nozzle_hole_solid, 0, G4ThreeVector(0., _up_nozzle_ypos, hole_zpos) );

    ics_energy_sph_solid = 
      new G4SubtractionSolid("ICS_ENERGY_SPH", ics_energy_sph_solid,
			     nozzle_hole_solid, 0, G4ThreeVector(0., _central_nozzle_ypos, hole_zpos) );

    ics_energy_sph_solid = 
      new G4SubtractionSolid("ICS_ENERGY_SPH", ics_energy_sph_solid,
			     nozzle_hole_solid, 0, G4ThreeVector(0., _down_nozzle_ypos, hole_zpos) );

    ics_energy_sph_solid = 
      new G4SubtractionSolid("ICS_ENERGY_SPH", ics_energy_sph_solid,
			     nozzle_hole_solid, 0, G4ThreeVector(0., _bottom_nozzle_ypos, hole_zpos) );


    G4Tubs* ics_energy_cyl_solid = 
      new G4Tubs("ICS_ENERGY_CYL",  _body_in_rad, _body_in_rad + _energy_thickness,  _energy_cyl_length/2., 0.*deg, 360.*deg);


    // Unions of parts
    G4double ics_tracking_zpos = _body_length/2. - _tracking_length/2.;
    G4UnionSolid* ics_solid = 
      new G4UnionSolid("ICS", ics_body_solid, ics_tracking_solid,
		       0, G4ThreeVector(0., 0., ics_tracking_zpos) );

    ics_solid = new G4UnionSolid("ICS", ics_solid, ics_energy_sph_solid,
				 0, G4ThreeVector(0., 0., _energy_sph_zpos) );

    G4double energy_cyl_zpos = -1. * (_body_length/2. + _energy_cyl_length/2.);
    ics_solid = new G4UnionSolid("ICS", ics_solid, ics_energy_cyl_solid,
				 0, G4ThreeVector(0., 0., energy_cyl_zpos) );

    G4LogicalVolume* ics_logic = 
      new G4LogicalVolume(ics_solid,
			  G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "ICS");

    //this->SetLogicalVolume(ics_logic);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), ics_logic, "ICS", _mother_logic, false, 0, false);

    
    ///// DB plugs placement
    G4Box* plug_solid = new G4Box("DB_PLUG", _plug_x/2., _plug_y/2., _plug_z/2.);
    G4LogicalVolume* plug_logic = new G4LogicalVolume(plug_solid,  MaterialsList::PEEK(), "DB_PLUG");
     _plug_posz = ics_tracking_zpos + _tracking_length/2. + _plug_z ;
    G4ThreeVector pos;
    for (int i=0; i<_num_DBs; i++) {
      pos = _DB_positions[i];
      pos.setY(pos.y()- 10.*mm);
      pos.setZ(_plug_posz);
      new G4PVPlacement(0, pos, plug_logic,
    			"DB_PLUG", _mother_logic, false, i, false);
    }



    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes copper_col = nexus::CopperBrown();
      //copper_col.SetForceSolid(true);
      ics_logic->SetVisAttributes(copper_col);
      G4VisAttributes dirty_white_col =nexus::DirtyWhite();
      dirty_white_col.SetForceSolid(true);
      plug_logic->SetVisAttributes(dirty_white_col); 

    }
    else {
      ics_logic->SetVisAttributes(G4VisAttributes::Invisible);
      plug_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }


    // VERTEX GENERATORS   //////////
    _body_gen = 
      new CylinderPointSampler(_body_in_rad, _body_length, _body_thickness, 0.);

    _tracking_gen = 
      new CylinderPointSampler(0.*cm, _tracking_length, _tracking_orad, 0.,
					     G4ThreeVector(0., 0., ics_tracking_zpos));

    _energy_cyl_gen = 
      new CylinderPointSampler(_body_in_rad, _energy_cyl_length, _energy_thickness, 0.,  G4ThreeVector(0., 0., energy_cyl_zpos));

    _energy_sph_gen = new SpherePointSampler(_energy_orad - _energy_thickness, _energy_thickness, G4ThreeVector(0., 0., _energy_sph_zpos),
					     0,	0., twopi, 180.*deg - _energy_theta, _energy_theta);


    _plug_gen = new BoxPointSampler(_plug_x, _plug_y, _plug_z,0.,
				    G4ThreeVector(0.,0.,0.),0);


    // Calculating some probs
    G4double body_vol = ics_body_solid->GetCubicVolume();
    G4double tracking_vol = ics_tracking_solid->GetCubicVolume();
    G4double energy_cyl_vol = ics_energy_cyl_solid->GetCubicVolume();
    G4double energy_sph_vol = ics_energy_sph_solid->GetCubicVolume();
    G4double total_vol = body_vol + tracking_vol + energy_cyl_vol + energy_sph_vol;

    _perc_body_vol = body_vol / total_vol;
    _perc_tracking_vol = (body_vol + tracking_vol) / total_vol;
    _perc_energy_cyl_vol = (body_vol + tracking_vol + energy_cyl_vol) / total_vol;
  }



  Next100Ics::~Next100Ics()
  {
    delete _body_gen;
    delete _plug_gen;
  }
  


  G4ThreeVector Next100Ics::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Vertex in the whole ICS volume
    if (region == "ICS") {

      G4double rand = G4UniformRand();

      if (rand < _perc_body_vol){
	vertex = _body_gen->GenerateVertex("BODY_VOL");        // Body
      }

   
      else if  (rand < _perc_tracking_vol){
	G4VPhysicalVolume *VertexVolume;
	do {
	  vertex = _tracking_gen->GenerateVertex("BODY_VOL");    // Tracking plane
	  // To check its volume, one needs to rotate and shift the vertex
	  // because the check is done using global coordinates
	  G4ThreeVector glob_vtx(vertex);
	  // First rotate, then shift
	  glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	  glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	  VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	} while (VertexVolume->GetName() != "ICS_TRACKING");
      }

      else if  (rand < _perc_energy_cyl_vol)
	vertex = _energy_cyl_gen->GenerateVertex("BODY_VOL");  // Energy plane, cylindric section

      else {
	G4VPhysicalVolume *VertexVolume;
	do {
	  vertex = _energy_sph_gen->GenerateVertex("VOLUME");     // Energy plane, spherical section
	  // To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	  VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	} while (VertexVolume->GetName() != "ICS");
      }
    }

    // PIGGY TAIL PLUG
    else if (region == "DB_PLUG") {
      G4ThreeVector ini_vertex = _plug_gen->GenerateVertex("INSIDE");
      G4double rand = _num_DBs * G4UniformRand();
      G4ThreeVector db_pos = _DB_positions[int(rand)];
      vertex = ini_vertex + db_pos;
      vertex.setY(vertex.y()- 10.*mm);
      vertex.setZ(vertex.z() + _plug_posz);
    }
 

    else {
      G4Exception("[Next100Ics]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");     
    }
     

    return vertex;
  }


  void Next100Ics::GenerateDBPositions()
  {
    /// Function that computes and stores the XY positions of Dice Boards

    G4int num_rows[] = {6, 9, 10, 11, 12, 11, 12, 11, 10, 9, 6};
    G4int total_positions = 0;

    // Separation between consecutive columns / rows
    G4double x_step = (45. + 35.191) * mm;
    G4double y_step = (45. + 36.718) * mm;

    G4double x_dim = x_step * (_DB_columns -1);

    G4ThreeVector position(0.,0.,0.);

    // For every column
    for (G4int col=0; col<_DB_columns; col++) {
      G4double pos_x = x_dim/2. - col * x_step;
      G4int rows = num_rows[col];
      G4double y_dim = y_step * (rows-1);
      // For every row
      for (G4int row=0; row<rows; row++) {
	G4double pos_y = y_dim/2. - row * y_step;

	position.setX(pos_x);
	position.setY(pos_y);
	_DB_positions.push_back(position);
	total_positions++;
	//G4cout << G4endl << position;
      }
    }

    // Checking
    if (total_positions != _num_DBs) {
      G4cout << "\n\nERROR: Number of DBs doesn't match with number of positions calculated\n";
      exit(0);
    }


  }


} //end namespace nexus
