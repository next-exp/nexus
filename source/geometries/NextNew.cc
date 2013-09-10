// ----------------------------------------------------------------------------
//  $Id: NextNew.cc  $
//
//  Author:  Miquel Nebot Guinot <miquel.nebot@ific.uv.es>    
//  Created: Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNew.h"

//#include "MaterialsList.h"


#include <G4GenericMessenger.hh>
#include <G4Box.hh>

//#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>


namespace nexus {

  NextNew::NextNew():
    BaseGeometry(),
    // Lab dimensions
    _lab_size (5. * m),
    // Buffer gas dimensions
    _buffer_gas_size (4. * m)
  {
    //Shielding
    //Vessel
    //ICS
    //Inner elements
  }

  NextNew::~NextNew()
  {
    //deletes
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

   
    // BUFFER GAS   ///////////////////////////////////////////////////////////////////////////////////
    // This is a volume, initially made of air, defined to be the mother volume of Shielding and Vessel

    G4Box* buffer_gas_solid = 
      new G4Box("BUFFER_GAS", _buffer_gas_size/2., _buffer_gas_size/2., _buffer_gas_size/2.);
    
    _buffer_gas_logic = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "BUFFER_GAS");
    _buffer_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);

    G4PVPlacement* buffer_gas_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), _buffer_gas_logic,
                                                       "BUFFER_GAS", _lab_logic, false, 0);


    //SHIELDING

    //VESSEL

    //ICS

    //INNER ELEMENTS

  }
    
  G4ThreeVector NextNew::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // //BUFFER GAS
    // if (region == "LAB") {
    //   vertex = _lab_gen->GenerateVertex(region);
    // }
    // //VESSEL REGIONS

    // // ICS REGIONS
    // else if (region == "ICS") {
    //   vertex = _ics_gen->GenerateVertex(region);
    // }
    // //Xenon Fiducial Volume
    // else 
    //   vertex=_xefv_gen->GenerateVertex("XEFV");
    
    return vertex;
  }
  
  
} //end namespace nexus
