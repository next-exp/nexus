// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 12 Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewFieldCage.h"
#include "MaterialsList.h"
#include <G4GenericMessenger.hh>
#include "OpticalMaterialProperties.h"

#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>

namespace nexus {

  NextNewFieldCage::NextNewFieldCage(): 
    BaseGeometry(),
    // Field cage dimensions
    _tube_in_rad (24.0 * cm),      
    _tube_length (58.0 * cm),   // Drift volume (51cm) + Buffer (7cm) 
    _tube_thickness (2.0 * cm), //=_vesse_in_rad -ics -_tube_in_rad     
    _reflector_thickness (0.1 * mm)  // Internal reflector thickness from EnergyPlane design - Dimension NEW design
  {
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("field_cage_vis", _visibility, "Field Cage Visibility");
  }

  void NextNewFieldCage::Construct()
  {
    // Body solid
    // _tube_z_pos = - (160.*cm/2. + 8.7*cm - _tube_length/2.);  //Vessel_body_len (160)+ Ener_zone displ(8.7)-

    G4Tubs* field_cage_solid = new G4Tubs("FIELD_CAGE", _tube_in_rad, _tube_in_rad + _tube_thickness,
					  _tube_length/2., 0, twopi);
    _hdpe = MaterialsList::HDPE();
    G4LogicalVolume* field_cage_logic = new G4LogicalVolume(field_cage_solid, _hdpe, "FIELD_CAGE");
    this->SetLogicalVolume(field_cage_logic);
   
    //Internal reflector
    G4Tubs* reflector_solid = new G4Tubs("FC_REFLECTOR", _tube_in_rad, _tube_in_rad  - _reflector_thickness,
					 _tube_length/2., 0, twopi);
    G4LogicalVolume* reflector_logic = new G4LogicalVolume(reflector_solid, _hdpe, "FC_REFLECTOR");
    G4PVPlacement* reflector_physi = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), reflector_logic, 
						       "FC_REFLECTOR", field_cage_logic, false, 0);

    /// OPTICAL SURFACE PROPERTIES    ////////
    G4OpticalSurface* reflector_opt_surf = new G4OpticalSurface("FC_REFLECTOR");
    reflector_opt_surf->SetType(dielectric_metal);
    reflector_opt_surf->SetModel(unified);
    reflector_opt_surf->SetFinish(ground);
    reflector_opt_surf->SetSigmaAlpha(0.1);
    reflector_opt_surf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
    new G4LogicalSkinSurface("FC_REFLECTOR", reflector_logic, reflector_opt_surf);

    /// SETTING VISIBILITIES   //////////
    // if (_visibility) {
    //   G4VisAttributes light_blue(G4Colour(0., 0., .7));
    //   G4VisAttributes blue(G4Colour(0., 0., 1.));
    //   //blue.SetForceSolid(true);
    //   //light_blue.SetForceSolid(true);

    //   field_cage_logic->SetVisAttributes(light_blue);
    //   reflector_logic->SetVisAttributes(blue);
    // }
    // else {
    //   field_cage_logic->SetVisAttributes(G4VisAttributes::Invisible);
    //   reflector_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // }

    /// VERTEX GENERATORS   //////////
    _body_gen  = new CylinderPointSampler(_tube_in_rad, _tube_length, _tube_thickness + _reflector_thickness,
					  0., G4ThreeVector (0., 0., _tube_z_pos));
  }

  NextNewFieldCage::~NextNewFieldCage()
  {
    delete _body_gen;
  }

  G4ThreeVector NextNewFieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    // Vertex in the plastic cylinder
    if (region == "FIELD_CAGE") {
      vertex = _body_gen->GenerateVertex(TUBE_VOLUME);
    }
    return vertex;
  }

  G4ThreeVector NextNewFieldCage::GetPosition() const
  {
    return G4ThreeVector (0., 0., _tube_z_pos);
  }


}//end namespace nexus

