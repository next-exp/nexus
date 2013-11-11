// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <paola.ferrario@ific.uv.es>, <jmunoz@ific.uv.es>
//  Created: 1 Mar 2012
//  
//  Copyright (c) 2012 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next100FieldCage.h"
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

  Next100FieldCage::Next100FieldCage(): 
    BaseGeometry(),
    // Field cage dimensions
    _tube_diam (106.9 * cm),       // Consistent with ACTIVE_diam
    //_tube_length (154.1 * cm),   // Mesh to Mesh dist (130 cm)  +  Ener Plane (25.1 cm)  -  Trk Plane (1 cm)
    _tube_length (154.34 * cm),    // Vessel body length (160 cm)  +  Ener Plane (8.7 cm)  -  Trk Plane (13.36 + 1 cm)
    _tube_thickn (2.54 * cm),      // Consistent with ACTIVE & reflector thicness

    // Internal reflector thickness
    _refl_thickn (.1 * mm)
  {

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");
    _msg->DeclareProperty("field_cage_vis", _visibility, "Field Cage Visibility");

  }



  void Next100FieldCage::Construct()
  {
    // Build the larger cylinder
    _tube_zpos = - (160.*cm/2. + 8.7*cm - _tube_length/2.);  // Considering Vessel_body_len = 160   &  Ener_zone displ = 8.7

    G4Tubs* field_cage_solid = new G4Tubs("FIELD_CAGE", _tube_diam/2., _tube_diam/2. + _tube_thickn,
					  _tube_length/2., 0, twopi);

    _hdpe = MaterialsList::HDPE();
    G4LogicalVolume* field_cage_logic = new G4LogicalVolume(field_cage_solid, _hdpe, "FIELD_CAGE");
    this->SetLogicalVolume(field_cage_logic);


    // Build the internal reflector
    G4Tubs* reflector_solid = new G4Tubs("FC_REFLECTOR", _tube_diam/2., _tube_diam/2.  + _refl_thickn,
					 _tube_length/2., 0, twopi);
    G4LogicalVolume* reflector_logic = new G4LogicalVolume(reflector_solid, _hdpe, "FC_REFLECTOR");
    G4PVPlacement* reflector_physi = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), reflector_logic, 
						       "FC_REFLECTOR", field_cage_logic, false, 0);


    // OPTICAL SURFACE PROPERTIES    ////////
    G4OpticalSurface* reflector_opsur = new G4OpticalSurface("FC_REFLECTOR");
    reflector_opsur->SetType(dielectric_metal);
    reflector_opsur->SetModel(unified);
    reflector_opsur->SetFinish(ground);
    reflector_opsur->SetSigmaAlpha(0.1);
    reflector_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
    new G4LogicalSkinSurface("FC_REFLECTOR", reflector_logic, reflector_opsur);


    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes light_blue(G4Colour(0., 0., .7));
      G4VisAttributes blue(G4Colour(0., 0., 1.));
      //blue.SetForceSolid(true);
      //light_blue.SetForceSolid(true);

      field_cage_logic->SetVisAttributes(light_blue);
      reflector_logic->SetVisAttributes(blue);
    }
    else {
      field_cage_logic->SetVisAttributes(G4VisAttributes::Invisible);
      reflector_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // VERTEX GENERATORS   //////////
    _body_gen  = new CylinderPointSampler(_tube_diam/2., _tube_length, _tube_thickn + _refl_thickn,
					  0., G4ThreeVector (0., 0., _tube_zpos));

  }



  Next100FieldCage::~Next100FieldCage()
  {
    delete _body_gen;
  }



  G4ThreeVector Next100FieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Vertex in the plastic cylinder
    if (region == "FIELD_CAGE") {
      vertex = _body_gen->GenerateVertex("BODY_VOLUME");
    }

    return vertex;

  }



  G4ThreeVector Next100FieldCage::GetPosition() const
  {
    return G4ThreeVector (0., 0., _tube_zpos);
  }


}


