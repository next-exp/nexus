// ----------------------------------------------------------------------------
//  $Id: NextNew.cc  $
//
//  Author:  Miquel Nebot Guinot <miquel.nebot@ific.uv.es>
//           <jmunoz@ific.uv.es>, <justo.martin-albo@ific.uv.es>   
//  Created: Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNew.h"

#include "Next100Shielding.h"
#include "NextNewPedestal.h"
//#include "NextNewCuCastle.h"
#include "NextNewVessel.h"
#include "NextNewIcs.h"
#include "NextNewInnerElements.h"
//#include "Na22Source.h"
#include "BoxPointSampler.h"
#include "MuonsPointSampler.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "IonizationSD.h"
#include "CalibrationSource.h"
#include "NaIScintillator.h"
#include "SurroundingAir.h"
#include "LeadCollimator.h"
#include "CollProtection.h"
#include "CollSupport.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>
#include <G4NistManager.hh>
#include <G4RotationMatrix.hh>
#include <G4Transform3D.hh>
#include <G4SDManager.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  NextNew::NextNew():
    BaseGeometry(),
    // Lab dimensions
    _lab_size (5. * m),
    _rot_angle(pi),
    _lead_block(false),
    _lead_dist(0.*mm),
    _ext_scint(false),
    _calib_pos(""),
    _dist_scint(25.*cm),
    _lead_castle(true)
    //   _ext_source_distance(0.*mm)
    // Buffer gas dimensions
  {
    //Shielding
    _shielding = new Next100Shielding();
    //Pedestal
    _pedestal = new NextNewPedestal();
    //Copper Castle
    // _cu_castle = new NextNewCuCastle();
    
    //Vessel
    _vessel = new NextNewVessel();
    //ICS
    _ics = new NextNewIcs();   
    //Inner elements
    _inner_elements = new NextNewInnerElements();

    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("lead_block", _lead_block, "Block of lead on the lateral port");
    _msg->DeclareProperty("lead_distance", _lead_dist, "Distance between the two blocks of lead");
    _msg->DeclareProperty("ext_scint", _ext_scint, "Placement of external NaI scintillator");
    _msg->DeclareProperty("calib_pos", _calib_pos, "Where calibration source is placed (lateral/axial)");
    _msg->DeclareProperty("scint_distance", _dist_scint, "Distance between the end of the lateral port tube and the external scintillator");
    _msg->DeclareProperty("lead_castle", _lead_castle, "Placement of lead castle");
    
    _cal = new CalibrationSource();
    _cal->Construct();

    _naI = new NaIScintillator();
    _naI->Construct();

    _air = new SurroundingAir();

    _coll = new LeadCollimator();
  }

  NextNew::~NextNew()
  {
    //deletes
    delete _shielding;
    delete _pedestal;
    // delete _cu_castle;
   
    delete _vessel;
    delete _ics;
    delete _inner_elements;
    
    delete _lab_gen;
    delete _lat_source_gen;
    delete _axial_source_gen;

    delete _muon_gen;
  }

  void NextNew::BuildExtScintillator(G4ThreeVector pos, const G4RotationMatrix& rot)
{ 
 
  G4LogicalVolume* sc_logic = _naI->GetLogicalVolume();

  G4double length = _naI->GetLength();

  G4ThreeVector pos_scint = 
    G4ThreeVector(pos.getX(), pos.getY(), pos.getZ()); 
  new G4PVPlacement(G4Transform3D(rot, pos_scint), sc_logic, "NaI",
		    _shielding_air_logic, false, 0, false);  

}

 void NextNew::Construct()
  {
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air surrounding the detector so that events 
    //(from calibration sources or cosmic rays) can be generated on the outside.
    
    G4Box* lab_solid = 
      new G4Box("LAB", _lab_size/2., _lab_size/2., _lab_size/2.);
    
    _lab_logic = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    
    _lab_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Set this volume as the wrapper for the whole geometry 
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(_lab_logic);

    G4LogicalVolume* shielding_logic;
    if (_lead_castle) {
      //SHIELDING
      _shielding->Construct();
      shielding_logic = _shielding->GetLogicalVolume();
      _shielding_air_logic = _shielding->GetAirLogicalVolume();
    } else {
      _air->Construct();
      _shielding_air_logic = _air->GetLogicalVolume();
    }
  
    //PEDESTAL
    _pedestal->SetLogicalVolume(_shielding_air_logic);
    _pedestal->Construct();
    
    //VESSEL
    _vessel->Construct();
    G4LogicalVolume* vessel_logic = _vessel->GetLogicalVolume();
    G4ThreeVector position(0.,0.,0.); 
    new G4PVPlacement(0, position, vessel_logic, 
		      "VESSEL", _shielding_air_logic, false, 0, false);
    G4LogicalVolume* vessel_gas_logic = _vessel->GetInternalLogicalVolume();

     //INNER ELEMENTS
    _inner_elements->SetLogicalVolume(vessel_gas_logic);
    _inner_elements->Construct();

    _shielding->SetELzCoord(_inner_elements->GetELzCoord());
    _vessel->SetELzCoord(_inner_elements->GetELzCoord());

     //ICS
    _ics->SetLogicalVolume(vessel_gas_logic);
    _ics->SetNozzlesZPosition( _vessel->GetLATNozzleZPosition(),_vessel->GetUPNozzleZPosition());
    _ics->SetELzCoord(_inner_elements->GetELzCoord());
    _ics->Construct();

    //COPPER CASTLE 
    // _cu_castle->SetLogicalVolume(_buffer_gas_logic);
    // _cu_castle->SetELzCoord(_inner_elements->GetELzCoord());
    // _cu_castle->Construct();

    G4ThreeVector lat_pos = _vessel->GetLatExtSourcePosition(); // this is the position of the end of the port tube
    G4RotationMatrix* lat_rot = new G4RotationMatrix();
    lat_rot->rotateY(-pi/2);

    G4ThreeVector axial_pos = _vessel->GetAxialExtSourcePosition(); // this is the position of the end of the port tube
    G4RotationMatrix* ax_rot = new G4RotationMatrix();
    ax_rot->rotateY(2*pi);

    /*
   /// In principle we shouldn't use this source anymore, but never say never...
    Na22Source* na22 = new Na22Source();
    na22->Construct();
    G4LogicalVolume* na22_logic = na22->GetLogicalVolume();

    // This is the position of the whole Na22 source + plastic support.
    G4ThreeVector lat_pos_source = G4ThreeVector(lat_pos.getX() + na22->GetSupportThickness()/2., lat_pos.getY(), lat_pos.getZ());

    new G4PVPlacement(G4Transform3D(*lat_rot, lat_pos_source), na22_logic, "NA22_SOURCE",
    		      _shielding_air_logic, false, 0, false);

    G4ThreeVector up_pos = _vessel->GetUpExtSourcePosition(); // this is the position of the end of the port tube
    G4RotationMatrix* up_rot = new G4RotationMatrix();
    up_rot->rotateX(pi/2.);

    G4ThreeVector up_pos_source = G4ThreeVector(up_pos.getX() , up_pos.getY() + na22->GetSupportThickness()/2., up_pos.getZ());

    new G4PVPlacement(G4Transform3D(*up_rot, up_pos_source), na22_logic, "NA22_SOURCE",
		      _shielding_air_logic, false, 1, false);

    G4VisAttributes light_brown_col = nexus::CopperBrown();
    na22_logic->SetVisAttributes(light_brown_col);
    */
    
    // Build NaI external scintillator

    if (_ext_scint) {
      if (_calib_pos == "lateral") {
        // G4RotationMatrix rot;
        // rot.rotateY(-pi/2.);
        BuildExtScintillator(G4ThreeVector(lat_pos.getX() + _naI->GetLength()/2. + _dist_scint,
                                           lat_pos.getY(), lat_pos.getZ()), *lat_rot);
      } else if (_calib_pos == "axial") {
        //    G4RotationMatrix rot_ax;
        BuildExtScintillator(G4ThreeVector(axial_pos.getX(), axial_pos.getY(),
                                           axial_pos.getZ() - _dist_scint - _naI->GetLength()/2.), *ax_rot);
      } else {
         G4Exception("[NextNew]", "Construct()", FatalException,
		  "The placement of external scintillator must be lateral or axial!");     
      }
    }

    G4ThreeVector source_pos;

    if (_lead_block) {

      _coll->Construct();
      G4LogicalVolume* coll_logic = _coll->GetLogicalVolume();
      G4double coll_centre = _coll->GetAxisCentre();

      CollProtection coll_protection;
      coll_protection.Construct();
      G4LogicalVolume* coll_protection_logic = coll_protection.GetLogicalVolume();
      G4double coll_protection_centre = coll_protection.GetAxisCentre();

      CollSupport coll_support;
      coll_support.Construct();
      G4LogicalVolume* coll_support_logic = coll_support.GetLogicalVolume();
      G4double coll_support_centre = coll_support.GetAxisCentre();

      if (_calib_pos == "lateral") {
        
        G4double vessel_out_diam = 664*mm;
        G4ThreeVector pos(vessel_out_diam/2. + coll_centre, lat_pos.getY(), lat_pos.getZ());
        new G4PVPlacement(G4Transform3D(*lat_rot, pos), coll_logic, "LEAD_COLLIMATOR",
                          _shielding_air_logic, false, 0, false);

        G4ThreeVector pos_protection(vessel_out_diam/2. + _coll->GetLength() + coll_protection.GetAxisCentre(),
                                     lat_pos.getY(), lat_pos.getZ());
        new G4PVPlacement(G4Transform3D(*lat_rot, pos_protection), coll_protection_logic, "SOURCE_PROTECTION",
                          _shielding_air_logic, false, 0, false);

        G4LogicalVolume* cal_logic = _cal->GetLogicalVolume();
        source_pos = G4ThreeVector(vessel_out_diam/2. + _coll->GetLength() - _cal->GetCapsuleThickness()/2.,
                                   lat_pos.getY(), lat_pos.getZ());
        new G4PVPlacement(G4Transform3D(*lat_rot, source_pos), cal_logic,
                          "SCREW_SUPPORT", _shielding_air_logic, false, 0, false);
        
      } else if (_calib_pos == "axial") {
        
        G4ThreeVector pos(axial_pos.getX(), axial_pos.getY(), axial_pos.getZ() - coll_centre);
        // new G4PVPlacement(0, pos, coll_logic, "LEAD_COLLIMATOR",
        //                   _shielding_air_logic, false, 0, true);
        G4ThreeVector pos_protection(axial_pos.getX(), axial_pos.getY(),
                                     axial_pos.getZ() - _coll->GetLength() - coll_protection.GetAxisCentre());
        new G4PVPlacement(0, pos_protection, coll_protection_logic, "SOURCE_PROTECTION",
                          _shielding_air_logic, false, 0, false);
        G4ThreeVector pos_support(axial_pos.getX(), axial_pos.getY() - coll_support.GetYDisplacement(),
                                     axial_pos.getZ() + coll_support.GetAxisCentre());
        new G4PVPlacement(0, pos_support, coll_support_logic, "SOURCE_SUPPORT",
                          _shielding_air_logic, false, 0, false);

        G4LogicalVolume* cal_logic = _cal->GetLogicalVolume();
        source_pos = G4ThreeVector(axial_pos.getX(), axial_pos.getY(),
                                   axial_pos.getZ() - _coll->GetLength() + _cal->GetCapsuleThickness()/2.);
       
        new G4PVPlacement(G4Transform3D(*ax_rot, source_pos), cal_logic,
                          "SCREW_SUPPORT", _shielding_air_logic, false, 0, false);
        
      } else {
        G4Exception("[NextNew]", "Construct()", FatalException,
                    "The placement of lead collimator must be lateral or axial!");     
      }

    }

    
    _displ = G4ThreeVector(0., 0., _inner_elements->GetELzCoord());
    G4RotationMatrix rot;
    rot.rotateY(_rot_angle);
    if (_lead_castle) {
      // Placement of the shielding volume, rotated and translated to have a right-handed ref system with z = z drift.
      new G4PVPlacement(G4Transform3D(rot, _displ), shielding_logic, "LEAD_BOX",
                        _lab_logic, false, 0, false);
    } else {
      new G4PVPlacement(G4Transform3D(rot, _displ), _shielding_air_logic, "AIR",
                        _lab_logic, false, 0, false);
    }


    //// VERTEX GENERATORS   //
    _lab_gen = 
      new BoxPointSampler(_lab_size - 1.*m, _lab_size - 1.*m, _lab_size - 1.*m, 1.*m,G4ThreeVector(0.,0.,0.),0);

    // this is the position of the source inside the capsule 
    G4ThreeVector gen_pos_lat = source_pos - G4ThreeVector(_cal->GetSourceZpos(), 0., 0.);
    G4ThreeVector gen_pos_axial = source_pos + G4ThreeVector(0, 0., _cal->GetSourceZpos());
    _lat_source_gen = new CylinderPointSampler(0., _cal->GetSourceThickness(), _cal->GetSourceDiameter()/2.,
                                               0., gen_pos_lat, lat_rot);

    _axial_source_gen = new CylinderPointSampler(0., _cal->GetSourceThickness(), _cal->GetSourceDiameter()/2.,
                                               0., gen_pos_axial, ax_rot);

    // G4double source_diam = na22->GetSourceDiameter();
    // G4double source_thick = na22->GetSourceThickness();
    // G4ThreeVector lat_pos_na22 =
    //   G4ThreeVector(lat_pos.getX() + na22->GetSourceThickness()/2., lat_pos.getY(), lat_pos.getZ());
    // G4ThreeVector up_pos_na22 =
    //   G4ThreeVector(up_pos.getX(), up_pos.getY() + na22->GetSourceThickness()/2., up_pos.getZ());

    // _source_gen_lat = new CylinderPointSampler(0., source_thick, source_diam/2., 0., lat_pos_na22, lat_rot);
    // _source_gen_up = new CylinderPointSampler(0., source_thick, source_diam/2., 0., up_pos_na22, up_rot);

    G4ThreeVector shielding_dim = _shielding->GetDimensions();

    _muon_gen =
      new MuonsPointSampler(shielding_dim.x()/2. + 50.*cm, shielding_dim.y()/2. + 1.*cm, shielding_dim.z()/2. + 50.*cm);

  }

 
    
  G4ThreeVector NextNew::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);
    //AIR AROUND SHIELDING
    if (region == "LAB") {
      vertex = _lab_gen->GenerateVertex("INSIDE");
    } else if (region == "MUONS") {
      vertex = _muon_gen->GenerateVertex();
    }
    else if (region == "EXTERNAL_PORT_ANODE") {
      vertex =  _lat_source_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "EXTERNAL_PORT_AXIAL") {
      vertex =  _axial_source_gen->GenerateVertex("BODY_VOL");
    }
    // else if (region == "NA22_PORT_UP_EXT") {
    //   vertex =  _source_gen_up->GenerateVertex("BODY_VOL");
    // }
    else if ( (region == "SHIELDING_LEAD") || (region == "SHIELDING_STEEL") || 
	      (region == "SHIELDING_GAS") || (region == "SHIELDING_STRUCT") ||
	      (region == "EXTERNAL") || (region == "SOURCE_PORT_AXIAL_EXT") ) {
      vertex = _shielding->GenerateVertex(region);   
    }
    //PEDESTAL
    else if (region == "PEDESTAL") {
      vertex = _pedestal->GenerateVertex(region);
    }
    //  //COPPER CASTLE
   // else if (region == "CU_CASTLE"){
   //   vertex = _cu_castle->GenerateVertex(region);
   //}
   //  //RADON 
   //  //on the inner lead surface (SHIELDING_GAS) and on the outer copper castle surface (RN_CU_CASTLE)
   // else if (region == "RN_CU_CASTLE") {
   //   vertex = _cu_castle->GenerateVertex(region);
   //  }

    //VESSEL REGIONS
    else if ( (region == "VESSEL") || 
	      (region == "SOURCE_PORT_ANODE") ||
	      (region == "SOURCE_PORT_UP") ||
	      (region == "SOURCE_PORT_AXIAL") ||
	      (region == "INTERNAL_PORT_ANODE") ){
      vertex = _vessel->GenerateVertex(region);
    }
    // ICS REGIONS
    else if (region == "ICS"){  
      vertex = _ics->GenerateVertex(region);
    }
    //INNER ELEMENTS
    else if ( (region == "CENTER") ||
	      (region == "CARRIER_PLATE") || (region == "ENCLOSURE_BODY") || (region == "ENCLOSURE_WINDOW") || 
	      (region == "OPTICAL_PAD") || (region == "PMT_BODY") || (region == "PMT_BASE") ||
	      (region == "DRIFT_TUBE") || (region == "ANODE_QUARTZ")||	     
	      (region == "ACTIVE") || (region == "EL_TABLE") || (region == "AD_HOC") || (region == "CATHODE")||
	      (region == "SUPPORT_PLATE") || (region == "DICE_BOARD") || (region == "DB_PLUG") ){
      vertex = _inner_elements->GenerateVertex(region);
    }
    else {
      G4Exception("[NextNew]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");     
    }

    // AD_HOC is the only vertex that is not rotated and shifted because it is passed by the user
    if  (region == "AD_HOC")
      return vertex;
    // First rotate, then shift
    vertex.rotate(_rot_angle, G4ThreeVector(0., 1., 0.));
    vertex = vertex + _displ;

    return vertex;
  }
  
  
} //end namespace nexus
