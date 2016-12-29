// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 14 Oct 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewIcs.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Colour.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>
#include <G4RotationMatrix.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

namespace nexus {

  using namespace CLHEP;
  
  NextNewIcs::NextNewIcs():/*const G4double lat_nozzle_z_pos,
			     const G4double up_nozzle_z_pos ):*/

    BaseGeometry(),

    // Body dimensions
    _body_out_diam (64.0  * cm), // inner diameter of the vessel
    _body_length  (68.4 * cm), /// nozzle to nozzle + flanges     ???????????
    _body_thickness (6.0 * cm),

    // Tracking plane tread dimensions 
    _tracking_tread_diam (55. * cm), //_support_plate_tread_diam from TP +.5
    _tracking_tread_length (84.1 * mm), //_support_plate_tread_thickness + _el_tot_zone 

   
    _lat_nozzle_in_diam  (5.*mm),//lat_nozzle_in_diam; before 30 mm
    _lat_nozzle_x_pos (_body_out_diam/2. -_body_thickness/2.),// lat_cathode_nozzle_xpos;
    _up_nozzle_in_diam (5.*mm), //up_nozzle_in_diam; before 60 mm
    _up_nozzle_y_pos (_lat_nozzle_x_pos)
    
  {
    // /// Needed External variables   
    // _lat_nozzle_z_pos = lat_nozzle_z_pos;
    // _up_nozzle_z_pos = up_nozzle_z_pos;
      
    // Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry Next100.");
    _msg->DeclareProperty("ics_vis", _visibility, "ICS Visibility");
    
  }
   
  void NextNewIcs::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  void NextNewIcs::SetNozzlesZPosition(const G4double lat_nozzle_z_pos, const G4double up_nozzle_z_pos)
  {
    _lat_nozzle_z_pos = lat_nozzle_z_pos;
    _up_nozzle_z_pos = up_nozzle_z_pos;
  }

  void NextNewIcs::Construct()
  {
    ////// INNER COPPER SHIELDING BARREL  ///////////

    G4Tubs* ics_body_nh_solid = 
      new G4Tubs("ICS_BODY", _body_out_diam/2. - _body_thickness,
		 _body_out_diam/2.- 1*mm,_body_length/2., 0., twopi);

    G4Tubs* ics_tracking_tread_solid = 
      new G4Tubs("ICS_TRACKING_TREAD", _body_out_diam/2. - _body_thickness - 2.*mm, _body_out_diam/2. - _body_thickness/2.,
		 _tracking_tread_length/2., 0., twopi); // _tracking_tread_diam/2.,

    G4SubtractionSolid* ics_solid =
      new G4SubtractionSolid("ICS", ics_body_nh_solid, ics_tracking_tread_solid,
			     0, G4ThreeVector(0., 0., _body_length/2. -_tracking_tread_length/2.));
       
    // NOZZLES HOLES //
    G4Tubs* lateral_nozzle_hole_solid = 
      new G4Tubs("LAT_NOZZLE_HOLE", 0., _lat_nozzle_in_diam/2., _body_thickness/2.+2.*mm, 0., twopi);
    
    //Rotate the nozzles holes
    G4RotationMatrix* roty = new G4RotationMatrix();
    roty->rotateY(pi/2.);
    
    //Lateral anode nozzle  
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, lateral_nozzle_hole_solid, 
   				       roty, G4ThreeVector(_lat_nozzle_x_pos, 0., -_lat_nozzle_z_pos) );
    //Lateral cathode nozzle
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, lateral_nozzle_hole_solid, 
   				       roty, G4ThreeVector(_lat_nozzle_x_pos, 0., _lat_nozzle_z_pos) );
    
    G4Tubs* up_nozzle_hole_solid =
      new G4Tubs("UP_NOZZLE_HOLE", 0., _up_nozzle_in_diam/2., _body_thickness/2.+5.*mm, 0., twopi);
    
    //Rotate the nozzles holes to vertical
    G4RotationMatrix* rotx = new G4RotationMatrix();
    rotx->rotateX(pi/2.);
    
    //Upper anode nozzle
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, up_nozzle_hole_solid,
				       rotx, G4ThreeVector( 0., _up_nozzle_y_pos, -_up_nozzle_z_pos) );
    //Upper central nozzle
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, up_nozzle_hole_solid,
				       rotx, G4ThreeVector( 0., _up_nozzle_y_pos, 0.) );

    //Upper cathode nozzle
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, up_nozzle_hole_solid,
				       rotx, G4ThreeVector( 0., _up_nozzle_y_pos, _up_nozzle_z_pos) );

    
    G4LogicalVolume* ics_logic = new G4LogicalVolume(ics_solid,
     						     G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "ICS");
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), ics_logic, "ICS", _mother_logic, false, 0, false);
   
    //this->SetLogicalVolume(ics_logic);


    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes copper_col = nexus::CopperBrown();
      copper_col.SetForceSolid(true);
      ics_logic->SetVisAttributes(copper_col);
    }
    else {
      ics_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // VERTEX GENERATORS   //////////
    _body_gen = new CylinderPointSampler( _body_out_diam/2. - _body_thickness, _body_length-_tracking_tread_length,
					  _body_thickness, 0., G4ThreeVector(0., 0., _tracking_tread_length) );
    _tread_gen = new CylinderPointSampler( _tracking_tread_diam/2., _tracking_tread_length,
					   _body_out_diam/2.-_tracking_tread_diam/2., 0., 
					   G4ThreeVector(0.,0.,-_body_length/2.+_tracking_tread_length/2.) );
		 
    // Calculating some probs
    G4double body_vol = 
      (_body_length-_tracking_tread_length)*pi*
      ((_body_out_diam/2.)*(_body_out_diam/2.)-(_body_out_diam/2.-_body_thickness)*(_body_out_diam/2. - _body_thickness));
    G4double tread_vol = 
      (_tracking_tread_length)*pi*((_body_out_diam/2.)*(_body_out_diam/2.)-(_tracking_tread_diam/2.)*( _tracking_tread_diam/2.));
    G4double total_vol = body_vol + tread_vol;
    _body_perc = body_vol/total_vol;
    //    std::cout<<"ICS VOLUME:\t"<<total_vol<<std::endl;
  }

  NextNewIcs::~NextNewIcs()
  {
    delete _body_gen;
    delete _tread_gen;
  }
  


  G4ThreeVector NextNewIcs::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "ICS") {
      G4double rand = G4UniformRand();

      //Generating in the body
      if (rand < _body_perc) {
        // As it is full of holes, let's get sure vertices are in the right volume
        G4VPhysicalVolume *VertexVolume;
        do {
          vertex = _body_gen->GenerateVertex("BODY_VOL");
	  // To check its volume, one needs to rotate and shift the vertex
	  // because the check is done using global coordinates
	  G4ThreeVector glob_vtx(vertex);
	  // First rotate, then shift
	  glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	  glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
          VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false);
        } while (VertexVolume->GetName() != "ICS");
      }
      // Generating in the tread
      else {
        vertex = _tread_gen->GenerateVertex("BODY_VOL");
      }     
    } else {
      G4Exception("[NextNewIcs]", "GenerateVertex()", FatalException,
		   "Unknown Region!");    
    }
    return vertex;
  }

} //end namespace nexus
