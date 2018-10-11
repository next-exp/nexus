// ----------------------------------------------------------------------------
//  $Id$
//
//  Copyright (c) 2018 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "ExtraVessel.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "PmtSD.h"
#include "CylinderPointSampler.h"
#include "Visibilities.h"

#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4RotationMatrix.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <Randomize.hh>
#include <G4OpticalSurface.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>


namespace nexus {
  
  using namespace CLHEP;
  
  ExtraVessel::ExtraVessel():
    BaseGeometry(),
    // Dimensions
    _diameter (10. * cm),
    _thickness (1. * mm),
    _visibility(1)
  {
    _msg = new G4GenericMessenger(this, "/Geometry/ExtraVessel/", "Control commands of ExtraVessel geometry.");
    _msg->DeclareProperty("visibility", _visibility, "visibility of materials placed outside the vessel, behind the tracking plane");
  }

  
   ExtraVessel::~ExtraVessel()
  {
    delete _generic_gen;
  }

  
  void ExtraVessel::Construct()
  {
    // GENERIC VOLUME //////////////////////////////////////////////////////

    G4Tubs* generic_solid = 
      new G4Tubs("EXTRA_VESSEL", 0., _diameter/2., _thickness/2., 
        0., twopi);

    // We model the material with a generic material of the density of FR4.
    // Feedthrough, adapter boards and connectors are made of FR4.
    // The O-rings are made of metal (some of them) and nitrile or viton.
    
    G4Material* mat = MaterialsList::FR4();

    G4LogicalVolume* generic_logic = 
      new G4LogicalVolume(generic_solid, mat, "EXTRA_VESSEL");
    this->SetLogicalVolume(generic_logic);


    // VISIBILITIES //////////////////////////////////////////////////
    if (_visibility) {
      G4VisAttributes generic_col = nexus::Yellow();
      generic_col.SetForceSolid(true);
      generic_logic->SetVisAttributes(generic_col);    
    } else {
      generic_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // VERTEX GENERATOR ////////////////////////////////////////////

    _generic_gen = new CylinderPointSampler(0, _thickness, _diameter/2., 0.,
					       G4ThreeVector (0., 0., 0.));
  }  


  G4ThreeVector ExtraVessel::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "EXTRA_VESSEL") {
        vertex = _generic_gen->GenerateVertex("BODY_VOL");
    }
    
    return vertex;
  }
  
} // end namespace nexus
