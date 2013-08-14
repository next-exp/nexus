// ----------------------------------------------------------------------------
//  $Id: GraXe.cc 2011-04-08 jmunoz $
//
//  Copyright (c) 2009 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "GraXe.h"

#include "ConfigService.h"
#include "IonizationSD.h"
#include "SpherePointSampler.h"
#include "MaterialsList.h"

#include <G4NistManager.hh>
#include <G4Orb.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <Randomize.hh>


namespace nexus {
  
  
  GraXe::GraXe(): BaseGeometry()
  {
    ReadParameters();
    BuildGeometry();

    // Creating the vertex generators

    G4double tolerance = 0.0001 * mm; // To generate SURFACE events inside the CU BALL
    _cu_ball_vertex_gen    = new SpherePointSampler(_out_active_radius+tolerance, _cu_ball_thickness-tolerance);

    _graph_ball_vertex_gen = new SpherePointSampler(_in_active_radius, _graph_ball_thickness);

    _snitch_vertex_gen     = new SpherePointSampler(0., _snitch_radius);

    _in_active_vertex_gen     = new SpherePointSampler(_snitch_radius, _in_active_radius - _snitch_radius);
  }
  
  
  
  GraXe::~GraXe()
  {
    delete _cu_ball_vertex_gen;
    delete _graph_ball_vertex_gen;
    delete _snitch_vertex_gen;
    delete _in_active_vertex_gen;
  }
  


  void GraXe::ReadParameters()
  {
    // get geometry configuration parameters
    const ParamStore& config = ConfigService::Instance().Geometry();

    // read dimensions
    _snitch_radius        = config.GetDParam("snitch_radius");
    _in_active_radius     = config.GetDParam("in_active_radius");
    _graph_ball_thickness = config.GetDParam("graph_ball_thickness");
    _out_active_radius    = config.GetDParam("out_active_radius");
    _cu_ball_thickness    = config.GetDParam("cu_ball_thickness");


    // read materials names
    G4String graph_ball_mat_name = config.GetSParam("graph_ball_material");
    G4String out_active_mat_name = config.GetSParam("out_active_material");
    G4String cu_ball_mat_name    = config.GetSParam("cu_ball_material");
    
    
    // Defining materials

    // Cu Ball Material
    if ((cu_ball_mat_name == "copper") or (cu_ball_mat_name == "Copper"))
    {
      _cu_ball_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    }
    else if ((cu_ball_mat_name == "titanium") or (cu_ball_mat_name == "Titanium"))
    {
      _cu_ball_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ti");
    }
    else
    {
      G4cout << "\n\nERROR: Material " << cu_ball_mat_name << " NOT ALLOWED for CU BALL";
      exit(0);
    }  

    // Graph Ball Material
    if ((graph_ball_mat_name == "kevlar") or (graph_ball_mat_name == "Kevlar"))
    {
      _graph_ball_mat = MaterialsList::Kevlar();
    }
    else
    {
      G4cout << "\n\nERROR: Material " << graph_ball_mat_name << " NOT ALLOWED for GRAPH BALL";
      exit(0);
    }  

    // Out Active Material
    if (out_active_mat_name == "LXe")
    {
      _out_active_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    }
    else if (out_active_mat_name == "LAr")
    {
      _out_active_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_lAr");
    }
    else
    {
      G4cout << "\n\nERROR: Material " << out_active_mat_name << " NOT ALLOWED for OUT ACTIVE";
      exit(0);
    }  

  }
  

  
  void GraXe::BuildGeometry()
  {

    // The CU BALL volume ...........................................
    G4double cu_ball_out_rad = _out_active_radius + _cu_ball_thickness;

    G4Orb* cu_ball_solid = new G4Orb("CU_BALL", cu_ball_out_rad);
    
    G4LogicalVolume* cu_ball_logic = 
      new G4LogicalVolume(cu_ball_solid, _cu_ball_mat, "CU_BALL");

    this->SetLogicalVolume(cu_ball_logic);


    
    // The OUT ACTIVE volume ...........................................
    G4Orb* out_active_solid = new G4Orb("OUT_ACTIVE", _out_active_radius);
    
    G4LogicalVolume* out_active_logic = new G4LogicalVolume(out_active_solid, _out_active_mat, "OUT_ACTIVE");
    out_active_logic->SetUserLimits(new G4UserLimits(2.*mm));

    G4PVPlacement* out_active_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), out_active_logic, 
							"OUT_ACTIVE", cu_ball_logic, false, 0);

    // OUT ACTIVE as sensitive detector
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    G4String SDname = "/GraXe/OUT_ACTIVE";
    G4String det_name = "out_active";
    IonizationSD* sd_out_active = new IonizationSD(SDname, det_name);
    //IonizationSD* sd_out_active = new IonizationSD(SDname);
    SDman->AddNewDetector(sd_out_active);
    out_active_logic->SetSensitiveDetector(sd_out_active);



    // The GRAPH BALL volume ...........................................
    G4double graph_ball_out_rad = _in_active_radius + _graph_ball_thickness;

    G4Orb* graph_ball_solid = new G4Orb("GRAPH_BALL", graph_ball_out_rad);
    
    G4LogicalVolume* graph_ball_logic = 
      new G4LogicalVolume(graph_ball_solid, _graph_ball_mat, "GRAPH_BALL");

    G4PVPlacement* graph_ball_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), graph_ball_logic, 
							"GRAPH_BALL", out_active_logic, false, 0);



    // The IN ACTIVE volume ...........................................
    G4Orb* in_active_solid = new G4Orb("IN_ACTIVE", _in_active_radius);
    
    G4LogicalVolume* in_active_logic = new G4LogicalVolume(in_active_solid,
							   G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe"),
							   "IN_ACTIVE");

    in_active_logic->SetUserLimits(new G4UserLimits(2.*mm));
    G4VisAttributes * visattribsolid = new G4VisAttributes;
    visattribsolid->SetForceSolid(true);
    in_active_logic->SetVisAttributes(visattribsolid);

    G4PVPlacement* in_active_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), in_active_logic, 
							"IN_ACTIVE", graph_ball_logic, false, 0);

    // IN ACTIVE as sensitive detector
    SDname = "/GraXe/IN_ACTIVE";
    det_name = "in_active";
    IonizationSD* sd_in_active = new IonizationSD(SDname, det_name);
    //IonizationSD* sd_in_active = new IonizationSD(SDname);
    SDman->AddNewDetector(sd_in_active);
    in_active_logic->SetSensitiveDetector(sd_in_active);



    // The SNITCH volume ...........................................
    G4Orb* snitch_solid = new G4Orb("SNITCH", _snitch_radius);
    
    G4LogicalVolume* snitch_logic = 
      new G4LogicalVolume(snitch_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "SNITCH");

    G4PVPlacement* snitch_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), snitch_logic, 
							"SNITCH", in_active_logic, false, 0);

  }
  
  

  G4ThreeVector GraXe::GenerateVertex(const G4String& region) const
  {
    if (region == "CENTER") {
      return G4ThreeVector(0., 0., 0.); 
    }

    if (region == "PRELIM") {
      return G4ThreeVector(-50.*cm, 0., 0.); 
    }

    // Generating in the SNITCH
    else if (region == "SNITCH") {
      return _snitch_vertex_gen->GenerateVertex("VOLUME");
    }

    // Generating in the IN ACTIVE
    else if (region == "IN_ACTIVE") {
      return _in_active_vertex_gen->GenerateVertex("VOLUME");
    }

    // Generating in the GRAPH_BALL
    else if (region == "GRAPH_BALL") {
      return _graph_ball_vertex_gen->GenerateVertex("VOLUME");
    }

    // Generating in the CU_BALL
    else if (region == "CU_BALL") {
      return _cu_ball_vertex_gen->GenerateVertex("VOLUME");
    }

    // Generating in the PMTs
    else if (region == "PMT") {
      return _cu_ball_vertex_gen->GenerateVertex("SURFACE");
    }


    // Generating in any part of the CHAMBER
    else {
      G4cout << "\n\n*** ERROR: " << region << " is not a valid region to generate events.\n\n";
      exit(0);
    }
  }
  

} // end namespace nexus
