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
#include <G4GenericMessenger.hh>

#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Cons.hh>
#include <G4Sphere.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Colour.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

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
    _tracking_hole_rad (6.0 * cm),     // To be checked

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
  

  
  void Next100Ics::Construct()
  {
    // ICS SOLIDS  ///////////

    // Body
    G4Tubs* ics_body_solid = new G4Tubs("ICS_BODY", _body_in_rad, _body_in_rad + _body_thickness,
					_body_length/2., 0.*deg, 360.*deg);


    // Tracking plane
    G4Tubs* ics_tracking_nh_solid = new G4Tubs("ICS_TRACKING_NH", 0.*cm, _tracking_orad,
    					       _tracking_length/2., 0.*deg, 360.*deg);

    G4Tubs* ics_tracking_hole_solid = new G4Tubs("ICS_TRACKING_HOLE", 0.*cm, _tracking_hole_rad,
    						 _tracking_length/2. + 5*cm, 0.*deg, 360.*deg);

    G4SubtractionSolid* ics_tracking_solid = new G4SubtractionSolid("ICS_TRACKING", ics_tracking_nh_solid,
    								    ics_tracking_hole_solid, 0, G4ThreeVector(0. , 0., 0.) );



    // Energy plane
    G4Sphere* ics_energy_sph_nh_solid = new G4Sphere("ICS_ENERGY_SPH_NH",
						     _energy_orad - _energy_thickness,  _energy_orad,   //radius
						     0. * deg, 360. * deg,                              // phi
						     180. * deg - _energy_theta, _energy_theta);        // theta
    
    G4double hole_diam = _nozzle_ext_diam + 1.*cm;
    G4double hole_length = _energy_thickness + 50.*cm;
    G4double hole_zpos = -1. * (_body_length/2. + hole_length/2.);

    G4Tubs* nozzle_hole_solid = new G4Tubs("NOZZLE_HOLE", 0.*cm, hole_diam/2., hole_length/2., 0.*deg, 360.*deg);

    G4SubtractionSolid* ics_energy_sph_solid = new G4SubtractionSolid("ICS_ENERGY_SPH", ics_energy_sph_nh_solid,
								      nozzle_hole_solid, 0, G4ThreeVector(0., _up_nozzle_ypos, hole_zpos) );

    ics_energy_sph_solid = new G4SubtractionSolid("ICS_ENERGY_SPH", ics_energy_sph_solid,
						  nozzle_hole_solid, 0, G4ThreeVector(0., _central_nozzle_ypos, hole_zpos) );

    ics_energy_sph_solid = new G4SubtractionSolid("ICS_ENERGY_SPH", ics_energy_sph_solid,
						  nozzle_hole_solid, 0, G4ThreeVector(0., _down_nozzle_ypos, hole_zpos) );

    ics_energy_sph_solid = new G4SubtractionSolid("ICS_ENERGY_SPH", ics_energy_sph_solid,
						  nozzle_hole_solid, 0, G4ThreeVector(0., _bottom_nozzle_ypos, hole_zpos) );


    G4Tubs* ics_energy_cyl_solid = new G4Tubs("ICS_ENERGY_CYL",  _body_in_rad, _body_in_rad + _energy_thickness,
					      _energy_cyl_length/2., 0.*deg, 360.*deg);


    // Unions of parts
    G4double ics_tracking_zpos = _body_length/2. + _tracking_length/2.;
    G4UnionSolid* ics_solid = new G4UnionSolid("ICS", ics_body_solid, ics_tracking_solid,
					       0, G4ThreeVector(0., 0., ics_tracking_zpos) );

    ics_solid = new G4UnionSolid("ICS", ics_solid, ics_energy_sph_solid,
				 0, G4ThreeVector(0., 0., _energy_sph_zpos) );

    G4double energy_cyl_zpos = -1. * (_body_length/2. + _energy_cyl_length/2.);
    ics_solid = new G4UnionSolid("ICS", ics_solid, ics_energy_cyl_solid,
				 0, G4ThreeVector(0., 0., energy_cyl_zpos) );

    G4LogicalVolume* ics_logic = new G4LogicalVolume(ics_solid,
     						     G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "ICS");

    this->SetLogicalVolume(ics_logic);


    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes copper_col(G4Colour(.72, .45, .20));
      copper_col.SetForceSolid(true);
      ics_logic->SetVisAttributes(copper_col);
    }
    else {
      ics_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }


    // VERTEX GENERATORS   //////////
    _body_gen = new CylinderPointSampler(_body_in_rad, _body_length, _body_thickness, 0.);

    _tracking_gen = new CylinderPointSampler(0.*cm, _tracking_length, _tracking_orad, 0.,
					     G4ThreeVector(0., 0., ics_tracking_zpos));

    _energy_cyl_gen = new CylinderPointSampler(_body_in_rad, _energy_cyl_length, _energy_thickness, 0.,
					       G4ThreeVector(0., 0., energy_cyl_zpos));

    _energy_sph_gen = new SpherePointSampler(_energy_orad - _energy_thickness, _energy_thickness, G4ThreeVector(0., 0., _energy_sph_zpos),
					     0,	0., twopi, 180.*deg - _energy_theta, _energy_theta);


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
  }
  


  G4ThreeVector Next100Ics::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Vertex in the whole ICS volume
    if (region == "ICS") {

      G4double rand = G4UniformRand();

      if (rand < _perc_body_vol)
	vertex = _body_gen->GenerateVertex("BODY_VOL");        // Body


      // (thin version without substractions)
      else if  (rand < _perc_tracking_vol)
	vertex = _tracking_gen->GenerateVertex("BODY_VOL");    // Tracking plane

      else if  (rand < _perc_energy_cyl_vol)
	vertex = _energy_cyl_gen->GenerateVertex("BODY_VOL");  // Energy plane, cylindric section

      else {
	G4VPhysicalVolume *VertexVolume;
	do {
	  vertex = _energy_sph_gen->GenerateVertex("VOLUME");     // Energy plane, spherical section
	  VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
	} while (VertexVolume->GetName() != "ICS");
      }
    }


    return vertex;
  }


} //end namespace nexus
