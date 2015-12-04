// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <justo.martin-albo@ific.uv.es>, <jmunoz@ific.uv.es>
//  Created: 21 Nov 2011
//  
//  Copyright (c) 2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next100Shielding.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>

#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4UnionSolid.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Colour.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>
#include <G4RotationMatrix.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  Next100Shielding::Next100Shielding(const G4double nozzle_ext_diam,
				     const G4double up_nozzle_ypos,
				     const G4double central_nozzle_ypos,
				     const G4double down_nozzle_ypos,
				     const G4double bottom_nozzle_ypos):
    BaseGeometry(),

    // Shielding internal dimensions
    _shield_x (155.  * cm),
    _shield_y (225.6 * cm),
    _shield_z (253.0 * cm),

    //Steel Structure
    _beam_base_thickness (4. *mm),
    _lateral_z_separation (1006. *mm), //distance between the two lateral beams
    _roof_z_separation (755. *mm), //distance between 1st and 2nd roof beams
    _front_x_separation (154.*mm), //distance between the two front beams
    // Box thickness
    _lead_thickness (20. * cm),
    _steel_thickness (6. * mm),
    _visibility (1)

  {

    /// Shielding is compound by two boxes, the external made of lead,
    /// and the internal, made of a mix of Steel & Titanium
    /// The Steel beam structure is placed inside the lead


    /// Needed External variables
    _nozzle_ext_diam = nozzle_ext_diam;
    _up_nozzle_ypos = up_nozzle_ypos;
    _central_nozzle_ypos = central_nozzle_ypos;
    _down_nozzle_ypos = down_nozzle_ypos;
    _bottom_nozzle_ypos = bottom_nozzle_ypos;
  

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");
    _msg->DeclareProperty("shielding_vis", _visibility, "Shielding Visibility");

    // Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  }


  
  void Next100Shielding::Construct()
  {
    // Auxiliary solids
    G4Box* shielding_box_solid = new G4Box("SHIELD_BOX", _shield_x/2., _shield_y/2., _shield_z/2.);
    G4Tubs* nozzle_hole_solid = new G4Tubs("NOZZLE_HOLE", 0.*cm, _nozzle_ext_diam/2.,
					   (_shield_z + 100.*cm)/2., 0.*deg, 360.*deg);

    // LEAD BOX   ///////////
    G4double lead_x = _shield_x + 2. * _steel_thickness + 2. * _lead_thickness;
    G4double lead_y = _shield_y + 2. * _steel_thickness + 2. * _lead_thickness;
    G4double lead_z = _shield_z + 2. * _steel_thickness + 2. * _lead_thickness;

    G4Box* lead_box_nh_solid = new G4Box("LEAD_BOX_NH", lead_x/2., lead_y/2., lead_z/2.);
    //make the services holes
    // G4SubtractionSolid* lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_nh_solid, nozzle_hole_solid,
    // 								0, G4ThreeVector(0. , _up_nozzle_ypos, 0.) );
    G4SubtractionSolid* lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_solid, nozzle_hole_solid,
					    0, G4ThreeVector(0., _central_nozzle_ypos, 0.) );
    // lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_solid, nozzle_hole_solid,
    // 					    0, G4ThreeVector(0., _down_nozzle_ypos, 0.) );
    // lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_solid, nozzle_hole_solid,
    // 					    0, G4ThreeVector(0., _bottom_nozzle_ypos, 0.) );

    lead_box_solid = new G4SubtractionSolid("LEAD_BOX", lead_box_solid, shielding_box_solid);


    G4LogicalVolume* lead_box_logic = new G4LogicalVolume(lead_box_solid,
							  G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"),
							  "LEAD_BOX");
    this->SetLogicalVolume(lead_box_logic);

    //STEEL BEAM STRUCTURE
    //auxiliar positions
    G4double lat_beam_x = _shield_x/2.+ _steel_thickness + _lead_thickness/2.; 
    G4double front_beam_z = _shield_z/2.+_steel_thickness+_lead_thickness/2.;
    //   G4double frontz = front_beam_z - (_roof_z_separation+_lateral_z_separation/2.);
    G4double top_beam_y = _shield_y/2.+ _steel_thickness + _lead_thickness/2.; 
    
    G4Box* roof_beam = new G4Box("STRUCT_BEAM",lead_x/2.,_beam_base_thickness/2.,lead_z/2.);
    G4Box* aux_box = new G4Box("AUX_box",_shield_x/2.+ _steel_thickness,_beam_base_thickness,_shield_z/2.+ _steel_thickness);
    G4SubtractionSolid* roof_beam_solid = new G4SubtractionSolid("STRUCT_BEAM",roof_beam,aux_box,0,G4ThreeVector(0.,0.,0.));
  
    G4Box* lat_beam_solid = new G4Box("STRUCT_BEAM",
				      _lead_thickness/2.,
				      _shield_y/2.+_steel_thickness/2.,
				      _beam_base_thickness/2.);
    G4Box* top_xbeam_solid = new G4Box("STRUCT_BEAM",
				      _shield_x/2.+_lead_thickness+_steel_thickness,
				      _lead_thickness/2.,
				      _beam_base_thickness/2.);
    G4Box* top_zbeam_solid = new G4Box("STRUCT_BEAM",
				       _beam_base_thickness/2.,
				       _lead_thickness/2. -1.*mm, 
				       _shield_z/2.+_lead_thickness+_steel_thickness);
    G4UnionSolid* struct_solid = new G4UnionSolid("STEEL_BEAM1_STRUCTURE",top_xbeam_solid,top_zbeam_solid,
						  0,G4ThreeVector(-_front_x_separation/2.,0.,
								  -(_roof_z_separation+_lateral_z_separation/2.)));
    struct_solid = new G4UnionSolid("STEEL_BEAM2_STRUCTURE",struct_solid,top_xbeam_solid,
      				    0,G4ThreeVector(0.,0., -_roof_z_separation));
    struct_solid = new G4UnionSolid("STEEL_BEAM3_STRUCTURE",struct_solid,top_xbeam_solid,
       				    0,G4ThreeVector(0.,0., -(_roof_z_separation+_lateral_z_separation)));
    struct_solid = new G4UnionSolid("STEEL_BEAM4_STRUCTURE",struct_solid,top_xbeam_solid,
       				    0, G4ThreeVector(0.,0., -(2*_roof_z_separation+_lateral_z_separation)));
    struct_solid = new G4UnionSolid("STEEL_BEAM5_STRUCTURE",struct_solid,top_zbeam_solid,
     				    0,G4ThreeVector(_front_x_separation/2.,0.,
      						    -(_roof_z_separation+_lateral_z_separation/2.)));
    
   
    G4LogicalVolume* roof_logic = new G4LogicalVolume(roof_beam_solid,
      						      MaterialsList::Steel(),
  						      "STEEL_BEAM_ROOF");
    G4LogicalVolume* lat_beam_logic = new G4LogicalVolume(lat_beam_solid,
     							  MaterialsList::Steel(),
 							  "STEEL_BEAM_STRUCTURE_LAT");
    G4LogicalVolume* struct_logic = new G4LogicalVolume(struct_solid,
							MaterialsList::Steel(),
							"STEEL_BEAM_STRUCTURE_TOP");
  
    new G4PVPlacement(0,G4ThreeVector(0.,_shield_y/2.+_steel_thickness/2.,0.),roof_logic,"STEEL_BEAM_STRUCTURE_roof",
		      lead_box_logic,false,0,false); 
    new G4PVPlacement(0,G4ThreeVector
		      (lat_beam_x,-_beam_base_thickness/2.,_lateral_z_separation/2.)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat1",
		      lead_box_logic,false,0,false); 
    new G4PVPlacement(0,G4ThreeVector
		      (lat_beam_x,-_beam_base_thickness/2.,-_lateral_z_separation/2.)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat2",
		      lead_box_logic,false,0,false); 
    new G4PVPlacement(0,G4ThreeVector
		      (-lat_beam_x,-_beam_base_thickness/2.,_lateral_z_separation/2.)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat3",
		      lead_box_logic,false,0,false); 
    new G4PVPlacement(0,G4ThreeVector
		      (-lat_beam_x,-_beam_base_thickness/2.,-_lateral_z_separation/2.)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat4",
		      lead_box_logic,false,0,false); 
    // //Rotate the beams
    G4RotationMatrix* rot_beam = new G4RotationMatrix();
    rot_beam->rotateY(pi/2.);    
    new G4PVPlacement(rot_beam,G4ThreeVector
		      (-_front_x_separation/2.,-_beam_base_thickness/2.,front_beam_z)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat5",
		      lead_box_logic,false,0,false); 
    new G4PVPlacement(rot_beam,G4ThreeVector
		      (_front_x_separation/2.,-_beam_base_thickness/2.,front_beam_z)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat6",
		      lead_box_logic,false,0,false); 
    new G4PVPlacement(rot_beam,G4ThreeVector
		      (-_front_x_separation/2.,-_beam_base_thickness/2.,-front_beam_z)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat7",
		      lead_box_logic,false,0,false); 
    new G4PVPlacement(rot_beam,G4ThreeVector
		      (_front_x_separation/2.,-_beam_base_thickness/2.,-front_beam_z)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat8",
		      lead_box_logic,false,0,false); 

    new G4PVPlacement(0,G4ThreeVector(0.,top_beam_y,_roof_z_separation+_lateral_z_separation/2.)
		      ,struct_logic,"STEEL_BEAM_STRUCTURE_top", lead_box_logic,false,0,false);
    
    
    // STEEL BOX   ///////////
    G4double steel_x = _shield_x + 2. * _steel_thickness;
    G4double steel_y = _shield_y + 2. * _steel_thickness;
    G4double steel_z = _shield_z + 2. * _steel_thickness;
    
    G4Box* steel_box_nh_solid = 
      new G4Box("STEEL_BOX_NH", steel_x/2., steel_y/2., steel_z/2.);
    G4SubtractionSolid* steel_box_solid = 
      new G4SubtractionSolid("STEEL_BOX", steel_box_nh_solid, nozzle_hole_solid,
			     0, G4ThreeVector(0. , _up_nozzle_ypos, 0.) );
    steel_box_solid = 
      new G4SubtractionSolid("STEEL_BOX", steel_box_solid, nozzle_hole_solid,
					     0, G4ThreeVector(0. , _central_nozzle_ypos, 0.) );
    steel_box_solid = 
      new G4SubtractionSolid("STEEL_BOX", steel_box_solid, nozzle_hole_solid,
					     0, G4ThreeVector(0. , _down_nozzle_ypos, 0.) );
    steel_box_solid = 
      new G4SubtractionSolid("STEEL_BOX", steel_box_solid, nozzle_hole_solid,
					     0, G4ThreeVector(0. , _bottom_nozzle_ypos, 0.) );
    steel_box_solid =
      new G4SubtractionSolid("STEEL_BOX", steel_box_solid, shielding_box_solid);

    G4LogicalVolume* steel_box_logic = new G4LogicalVolume(steel_box_solid,
							   MaterialsList::Steel(),
							   "STEEL_BOX");

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), steel_box_logic,
		      "STEEL_BOX", lead_box_logic, false, 0);


    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes dark_grey_col = nexus::DarkGrey();
      lead_box_logic->SetVisAttributes(dark_grey_col);
      G4VisAttributes dark_red_col = nexus::DarkRed();
      steel_box_logic->SetVisAttributes(dark_red_col);
      G4VisAttributes antiox_col = nexus::BloodRed();
      //  antiox.SetForceSolid(true);
      roof_logic->SetVisAttributes(antiox_col);
      lat_beam_logic->SetVisAttributes(antiox_col);
      struct_logic->SetVisAttributes(antiox_col);
    }
    else {
      lead_box_logic->SetVisAttributes(G4VisAttributes::Invisible);
      steel_box_logic->SetVisAttributes(G4VisAttributes::Invisible);
      roof_logic->SetVisAttributes(G4VisAttributes::Invisible);
      struct_logic->SetVisAttributes(G4VisAttributes::Invisible);
      lat_beam_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // Creating the vertex generators   //////////
    //_lead_gen  = new BoxPointSampler(steel_x, steel_y, steel_z, _lead_thickness, G4ThreeVector(0.,0.,0.), 0);
    _lead_gen  = 
      new BoxPointSampler(steel_x, steel_y, steel_z, 5.*cm, G4ThreeVector(0.,0.,0.), 0);
    _steel_gen = new BoxPointSampler(_shield_x, _shield_y, _shield_z, _steel_thickness, G4ThreeVector(0.,0.,0.), 0);
    G4double offset = 1.*cm;
    _external_gen = 
      new BoxPointSampler(lead_x + offset, lead_y + offset, lead_z + offset,
			  1.*mm, G4ThreeVector(0.,0.,0.), 0);


    _lat_roof_gen = 
      new BoxPointSampler(_lead_thickness,_beam_base_thickness,_shield_z,0., 
			  G4ThreeVector(0.,_shield_y/2.+_steel_thickness/2.,0.),0);
    _front_roof_gen = 
      new BoxPointSampler(lead_x,_beam_base_thickness,_lead_thickness,0., 
			  G4ThreeVector(0.,_shield_y/2.+_steel_thickness/2.,0.),0);
    // _struct_gen=;
    _struct_x_gen = new BoxPointSampler((_shield_x+2*_lead_thickness+2*_steel_thickness), _lead_thickness, _beam_base_thickness,0.,
					G4ThreeVector(0.,top_beam_y,_roof_z_separation+_lateral_z_separation/2),0);
    _struct_z_gen = new BoxPointSampler( _beam_base_thickness, _lead_thickness -1.*mm, _shield_z+2*_lead_thickness+2*_steel_thickness,0.,
					 G4ThreeVector(-_front_x_separation/2.,top_beam_y, 0.), 0);
    _lat_beam_gen = new BoxPointSampler(_lead_thickness, _shield_y+_steel_thickness,_beam_base_thickness,0.,
					G4ThreeVector(lat_beam_x,-_beam_base_thickness/2.,_lateral_z_separation/2.),0);
    _front_beam_gen = new BoxPointSampler(_beam_base_thickness, _shield_y+_steel_thickness,_lead_thickness,0.,
					G4ThreeVector(-_front_x_separation/2.,-_beam_base_thickness/2.,front_beam_z),0);
   
    
 
    // Calculating some probs
    G4double roof_vol = roof_beam_solid->GetCubicVolume();
    //std::cout<<"ROOF BEAM VOLUME "<<roof_vol<<std::endl;   
    G4double struct_top_vol = struct_solid->GetCubicVolume();
    //std::cout<<"TOP STRUCT VOLUME "<<struct_top_vol<<std::endl;
    G4double lateral_vol = lat_beam_solid->GetCubicVolume();
    //std::cout<<"LAT BEAM STRUCT VOLUME "<<lateral_vol<<"\t TOTAL LATERAL BEAMS VOL "<<8*lateral_vol<<std::endl;
    G4double total_vol = roof_vol+struct_top_vol+(8*lateral_vol);
    //std::cout<<"TOTAL STRUCTURE VOLUME "<<total_vol<<std::endl;

    _perc_roof_vol = roof_vol/total_vol; 
    _perc_front_roof_vol = 2*(lead_x*_beam_base_thickness*_lead_thickness) /roof_vol;
    _perc_top_struct_vol = struct_top_vol /total_vol;
    _perc_struc_x_vol = 4*((_shield_x+2*_lead_thickness+2*_steel_thickness)*_lead_thickness*_beam_base_thickness)/struct_top_vol; 

    // std::cout<<"SHIELDING LEAD VOLUME:\t"<<lead_box_solid->GetCubicVolume()<<std::endl;
    // std::cout<<"SHIELDING STEEL VOLUME:\t"<<steel_box_solid->GetCubicVolume()<<std::endl;
    // std::cout<<"VOLUME INSIDE THE SHIELDING CASTLE:\t"<<shielding_box_solid->GetCubicVolume()<<std::endl;
    
  }


  
  Next100Shielding::~Next100Shielding()
  {
    delete _lead_gen;
    delete _steel_gen;
    delete _external_gen;
    delete _lat_roof_gen;
    delete _front_roof_gen;
    delete _struct_x_gen;
    delete _struct_z_gen;
    delete _lat_beam_gen;
    delete _front_beam_gen;
  }



  G4ThreeVector Next100Shielding::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

   
    if (region == "SHIELDING_LEAD") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _lead_gen->GenerateVertex("WHOLE_VOL");
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "LEAD_BOX");
    }

    else if (region == "SHIELDING_STEEL") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _steel_gen->GenerateVertex("WHOLE_VOL");
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "STEEL_BOX");
    }

    else if (region == "SHIELDING_GAS") {
      vertex = _steel_gen->GenerateVertex("WHOLE_SURF");
    }

    else if (region == "EXTERNAL") {
      vertex = _external_gen->GenerateVertex("WHOLE_VOL");
    }
    else if(region=="SHIELDING_STRUCT"){
      G4double rand = G4UniformRand();

      if (rand < _perc_roof_vol) { //ROOF BEAM STRUCTURE
      	// G4VPhysicalVolume *VertexVolume;
      	// do {
      	if (G4UniformRand() <  _perc_front_roof_vol){
      	  if (G4UniformRand() < 0.5) {
      	    vertex = _front_roof_gen->GenerateVertex("INSIDE");
      	    vertex.setZ(vertex.z() + (_shield_z/2.+_steel_thickness+_lead_thickness/2.));
	    // std::cout<<"frontal +"<<std::endl;
      	  }	  
      	  else{
      	    vertex = _front_roof_gen->GenerateVertex("INSIDE");
      	    vertex.setZ(vertex.z() - (_shield_z/2.+_steel_thickness+_lead_thickness/2.));
      	    // std::cout<<"frontal -"<<std::endl;
      	  }
      	}
      	else{
      	  if (G4UniformRand() < 0.5) {
      	    vertex = _lat_roof_gen->GenerateVertex("INSIDE");	   
      	    vertex.setX(vertex.x() + ( _shield_x/2.+ _steel_thickness + _lead_thickness/2.));
      	    // std::cout<<"lateral +"<<std::endl;
      	  }	  
      	  else{
      	    vertex = _lat_roof_gen->GenerateVertex("INSIDE");	   
      	    vertex.setX(vertex.x() - ( _shield_x/2.+ _steel_thickness + _lead_thickness/2.));
      	    // std::cout<<"lateral -"<<std::endl;
      	  }
      	}
      	// VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      	// } while (VertexVolume->GetName() != "STEEL_BEAM_ROOF");
      }

      else if (rand < _perc_top_struct_vol) { //TOP BEAM STRUCTURE
	G4double random = G4UniformRand();
	if (random <  _perc_struc_x_vol){
	  G4double rand_beam = int (4* G4UniformRand());
	  if (rand_beam == 0) {
	    vertex = _struct_x_gen->GenerateVertex("INSIDE");
	  }
	  else if (rand_beam == 1) {
	    vertex = _struct_x_gen->GenerateVertex("INSIDE");
	    vertex.setZ(vertex.z()-_roof_z_separation);
	  }
	  else if (rand_beam == 2) {
	    vertex = _struct_x_gen->GenerateVertex("INSIDE");
	    vertex.setZ(vertex.z()-(_roof_z_separation+_lateral_z_separation));
	  }
	  else if (rand_beam == 3) {
	    vertex = _struct_x_gen->GenerateVertex("INSIDE");
	    vertex.setZ(vertex.z()-(2*_roof_z_separation+_lateral_z_separation));
	  }
	}
	else {
	  if (G4UniformRand() < 0.5) {
	    vertex = _struct_z_gen->GenerateVertex("INSIDE");
	  }
	  else {
	    vertex = _struct_z_gen->GenerateVertex("INSIDE");
	    vertex.setX(vertex.x()+_front_x_separation);
	  }
	}
      }

      else{    //LATERAL BEAM STRUCTURE
	G4double rand_beam = int (8 * G4UniformRand());
	// std::cout<< "viga numero "<<rand_beam<<std::endl; //0-7
	if (rand_beam == 0) {
	  //lat_1 (lat_beam_x,-_beam_base_thickness/2.,_lateral_z_separation/2.)
	  vertex = _lat_beam_gen->GenerateVertex("INSIDE");
	}
	else if (rand_beam ==1){
	  // //lat_2 (lat_beam_x,-_beam_base_thickness/2.,-_lateral_z_separation/2.)
	  vertex = _lat_beam_gen->GenerateVertex("INSIDE");
	  vertex.setZ(vertex.z() -_lateral_z_separation);  
	}
	else if (rand_beam ==2){
	  // //lat_3 	(-lat_beam_x,-_beam_base_thickness/2.,_lateral_z_separation/2.)
	  vertex = _lat_beam_gen->GenerateVertex("INSIDE");
	  vertex.setX(vertex.x() -(_shield_x + 2*_steel_thickness + _lead_thickness ));  
	}
	else if (rand_beam ==3){
	  // //lat_4 (-lat_beam_x,-_beam_base_thickness/2.,-_lateral_z_separation/2.)      
	  vertex = _lat_beam_gen->GenerateVertex("INSIDE");
	  vertex.setX(vertex.x() -(_shield_x + 2*_steel_thickness + _lead_thickness ));  
	  vertex.setZ(vertex.z() -_lateral_z_separation);  
	}
	else if (rand_beam ==4){
	  // //lat_5 front_beam (-_front_x_separation/2.,-_beam_base_thickness/2.,front_beam_z)
	  vertex = _front_beam_gen->GenerateVertex("INSIDE");
	}
	else if (rand_beam ==5){
	  // //lat_6 front_beam (_front_x_separation/2.,-_beam_base_thickness/2.,front_beam_z)
	  vertex = _front_beam_gen->GenerateVertex("INSIDE");
	  vertex.setX(vertex.x() + _front_x_separation);
	}
	else if (rand_beam ==6){
	  // //lat_7 front_beam (-_front_x_separation/2.,-_beam_base_thickness/2.,-front_beam_z)
	  vertex = _front_beam_gen->GenerateVertex("INSIDE");
	  vertex.setZ(vertex.z() -(_shield_z+2*_steel_thickness+_lead_thickness));
	}
	else if (rand_beam ==7){
	  //lat_8 front_beam (_front_x_separation/2.,-_beam_base_thickness/2.,-front_beam_z)
	  vertex = _front_beam_gen->GenerateVertex("INSIDE");
	  vertex.setX(vertex.x() + _front_x_separation);
	  vertex.setZ(vertex.z() -(_shield_z+2*_steel_thickness+_lead_thickness));
	}
      }   

    }
    else {
      G4Exception("[Next100Shielding]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");     
    }
    
    return vertex;
  }


} //end namespace nexus
