// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  Miryam Martínez Vara <Miryam.Martinez@ific.uv.es>    
//  Created: 7 May 2020
//  
//  Copyright (c) 2020 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "Test_Geometry.h"
#include "Giant_detector.h"

#include "BaseGeometry.h"
#include "SpherePointSampler.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4GenericMessenger.hh>
#include <G4Orb.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <Randomize.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4OpticalSurface.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {
  
  using namespace CLHEP;
  
  Test_Geometry::Test_Geometry():
    _world_size (10. * m),
    _tpb_diam_outer (20. * cm), 
    _tpb_diam_inner (0. * cm),
    //_tpb_length (1. * micrometer),
    _tpb_length (2. * micrometer),
    //_pedot_diam_outer (20. * cm), 
    //_pedot_diam_inner (0. * cm),
    //_pedot_length (200. * nanometer),
    _pedot_diam_outer (0.), 
    _pedot_diam_inner (0.),
    _pedot_length (0.),
    _sapphire_diam_outer (20. * cm), 
    _sapphire_diam_inner (0. * cm),
    _sapphire_length (6. * mm),

    //GAS:
    _gas ("naturalXe"),
    _pressure (15. * bar),
    _temperature (303. * kelvin),
    _helium_mass_num (4),
    _xe_perc (100.),

    _visibility(0)
  {
    _msg = new G4GenericMessenger(this, "/Geometry/Test_Geometry/",
				  "Control commands of Test_Geometry.");
    _msg->DeclareProperty("visibility", _visibility, "Giant detectors visibility");

    G4GenericMessenger::Command&  specific_vertex_X_cmd =
      _msg->DeclareProperty("specific_vertex_X", _specific_vertex_X,
                            "If region is AD_HOC, x coord of primary particles");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
      _msg->DeclareProperty("specific_vertex_Y", _specific_vertex_Y,
                            "If region is AD_HOC, y coord of primary particles");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
      _msg->DeclareProperty("specific_vertex_Z", _specific_vertex_Z,
                            "If region is AD_HOC, z coord of primary particles");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");
  }
  
  
  
  Test_Geometry::~Test_Geometry()
  {
    delete _msg;
  }
    
  
  
  void Test_Geometry::Construct()
  {
  // WORLD /////////////////////////////////////////////////

  G4String world_name = "WORLD";
  
  G4Material* world_mat = nullptr;
    if (_gas == "naturalXe") {
      world_mat = MaterialsList::GXe(_pressure, _temperature);
      world_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure,
                                                                           _temperature));

    } else if (_gas == "enrichedXe") {
      world_mat =  MaterialsList::GXeEnriched(_pressure, _temperature);
      world_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure,
                                                                           _temperature));

    } else if  (_gas == "depletedXe") {
      world_mat =  MaterialsList::GXeDepleted(_pressure, _temperature);
      world_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure,
                                                                           _temperature));

    } else if  (_gas == "XeHe") {
      world_mat = MaterialsList::GXeHe(_pressure, 300. * kelvin,  _xe_perc, _helium_mass_num);
      world_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure,
                                                                           300. * kelvin,
					                                   _xe_perc,
                                                                           _helium_mass_num));

    } else {
      G4Exception("[WORLD Test_Geometry]", "Construct()", FatalException,
		  "Unknown kind of xenon, valid options are: natural, enriched, depleted, or XeHe.");
    }

  G4Box* world_solid_vol =
    new G4Box(world_name, _world_size/2., _world_size/2., _world_size/2.);

  G4LogicalVolume* world_logic_vol =
    new G4LogicalVolume(world_solid_vol, world_mat, world_name);
  world_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
  BaseGeometry::SetLogicalVolume(world_logic_vol);
  
  // TPB //////////////////////////////////////////////

  G4String tpb_name = "TPB";
 
  G4Material* tpb_mat = MaterialsList::TPB();
  tpb_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());

  G4Tubs* tpb_solid_vol =
    new G4Tubs(tpb_name, _tpb_diam_inner/2, _tpb_diam_outer/2., _tpb_length/2., 0., 360.*deg);
  
  G4LogicalVolume* tpb_logic_vol =
    new G4LogicalVolume(tpb_solid_vol, tpb_mat, tpb_name);

  // Optical properties
  
  //G4OpticalSurface* tpb_opt_surf = GetPhotOptSurf();

  //new G4LogicalSkinSurface(tpb_name, tpb_logic_vol, tpb_opt_surf);

  G4OpticalSurface* tpb_opt_surf = 
    new G4OpticalSurface("gas_tpb_surf", glisur, ground, dielectric_dielectric, 0.01);

  new G4LogicalSkinSurface(tpb_name, tpb_logic_vol, tpb_opt_surf);

  G4VisAttributes tpb_color = nexus::Blue();
  tpb_color.SetForceSolid(true);
  tpb_logic_vol->SetVisAttributes(tpb_color);

  new G4PVPlacement(0, G4ThreeVector(0.,0.,-_tpb_length/2),
                    tpb_logic_vol, tpb_name, world_logic_vol, false, 0, true);

  // PEDOT ///////////////////////////////////////////////////

  //G4String pedot_name = "PEDOT";

  //G4Material* pedot_mat = MaterialsList::PEDOT();
  //pedot_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::PEDOT());

  //G4Tubs* pedot_solid_vol =
    //new G4Tubs(pedot_name, _pedot_diam_inner/2, _pedot_diam_outer/2., _pedot_length/2., 0., 360.*deg);
  
  //G4LogicalVolume* pedot_logic_vol =
    //new G4LogicalVolume(pedot_solid_vol, pedot_mat, pedot_name);

  //G4VisAttributes pedot_color = nexus::Brown();
  //pedot_color.SetForceSolid(true);
  //pedot_logic_vol->SetVisAttributes(pedot_color); 

  //new G4PVPlacement(0, G4ThreeVector(0.,0.,-_tpb_length-_pedot_length/2),
                    //pedot_logic_vol, pedot_name, world_logic_vol, false, 0, true);

  // SAPPHIRE ///////////////////////////////////////////////////

  G4String sapphire_name = "SAPPHIRE";
 
  G4Material* sapphire_mat = MaterialsList::Sapphire();
  sapphire_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::Sapphire());

  G4Tubs* sapphire_solid_vol =
  new G4Tubs(sapphire_name, _sapphire_diam_inner/2, _sapphire_diam_outer/2., _sapphire_length/2., 0., 360.*deg);
  
  G4LogicalVolume* sapphire_logic_vol =
    new G4LogicalVolume(sapphire_solid_vol, sapphire_mat, sapphire_name);

  G4VisAttributes sapphire_color = nexus::LightGrey();
  sapphire_color.SetForceSolid(true);
  sapphire_logic_vol->SetVisAttributes(sapphire_color);

  new G4PVPlacement(0, G4ThreeVector(0.,0.,-_tpb_length-_pedot_length-_sapphire_length/2),
                    sapphire_logic_vol, sapphire_name, world_logic_vol, false, 0, true);

 
  //////////////////////////////////////////////////////////

  Giant_detector detector;
  detector.Construct();
  G4LogicalVolume* detector_logic_vol = detector.GetLogicalVolume();

  G4double position_2_detector = 1.5*mm;

  new G4PVPlacement(0, G4ThreeVector(0.,0.,4.*mm),
                    detector_logic_vol, "FRONT_DETECTOR",
                    world_logic_vol, false, 0, true);

  new G4PVPlacement(0, G4ThreeVector(0.,0.,-_tpb_length-_pedot_length-_sapphire_length-position_2_detector),
                    detector_logic_vol, "BACK_DETECTOR",
                    world_logic_vol, false, 1, true);
  
  // VISIBILITIES ///////////////////////////////////////////////////
    
    if (_visibility) {
      detector_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
    } 
  }

    G4ThreeVector Test_Geometry::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // WORLD
    if (region == "WORLD") {
 
      vertex = G4ThreeVector(0.,0.,1.5*mm);
 
    }
    else if (region == "AD_HOC") {
      // AD_HOC does not need to be shifted because it is passed by the user
      vertex = G4ThreeVector(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
      return vertex;
    }
    else {
      G4Exception("[Test_Geometry]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    G4ThreeVector displacement = G4ThreeVector(0., 0., 0.); 
    vertex = vertex + displacement;

    return vertex;
  }




  G4OpticalSurface* Test_Geometry::GetPhotOptSurf() 
  {
	
  }
  

} // end namespace nexus
