// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <jmunoz@ific.uv.es>
//  Created: 25 Apr 2012
//  
//  Copyright (c) 2012 NEXT Collaboration
//
//  Updated to NextDemo++  by  Ruth Weiss Babai <ruty.wb@gmail.com>
//  From:   "Next100EnergyPlane.cc"
//  Date:       June 2019
// ---------------------------------------------------------------------------- 

#include "NextDemoEnergyPlane.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <G4VPhysicalVolume.hh>
#include <G4TransportationManager.hh>
#include <Randomize.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  NextDemoEnergyPlane::NextDemoEnergyPlane():

    num_PMTs_ (3),
    energy_plane_posz_ (-209.5 * mm),  //  from STEP-FILE:: Sapphire PosZ (-21.55 * cm)

    // Carrier Plate dimensions
    // STEP file:  0610-3  PMTs  FLANGE_PMTs::
    carrier_plate_thickness_ (45. * mm),
     // OUTER ACTIVE is 229*mm // (194.227mm) It must be consistent with ACTIVE diameter (from drawings)/(194.97mm)/160mm in Next1El
    carrier_plate_diam_ ( 234. * mm), //WidthOut 280.003 mm,thick 23.049 mm,netRAD 233.905 mm
    carrier_plate_central_hole_diam_ (10. * mm), // It is instead of central OLD-PMT(It is 10cm for Next100: nozzle_external_diam + 1 cm(equal to the ICS hole))  //  external hole only with NO extra 1 cm, here. //  16 mm or 10 mm

    // Ruty - from  Next100EnergyPlane.cc
    // Enclosures dimensions - like in Next100 ? Since they are the same PMT's
    enclosure_length_ (18.434 * cm),
    enclosure_diam_ (9.6 * cm),
    enclosure_flange_length_ (19.5 * mm),
    enclosure_window_thickness_ (7.1 * mm),
    enclosure_window_diam_ (85. * mm), 
    enclosure_pad_thickness_ (1.0 * mm),         
    pmt_base_diam_ (47. *mm),
    pmt_base_thickness_ (5. *mm),
    tpb_thickness_ (1.*micrometer),
    //   _pmts_pitch (11.0 * cm),
    visibility_(1),
    verbosity_(0)
  {
    // //////////////////////////////////////////////////////////////////////////

    /// Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();


    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/", "Control commands of geometry NextDemo.");
    msg_->DeclareProperty("energy_plane_vis", visibility_, "Energy Plane Visibility");

    msg_->DeclareProperty("energy_plane_verbosity", verbosity_, "Energy Plane verbosity");

    /// The PMT
    pmt_ = new PmtR11410(); 

  }



  void NextDemoEnergyPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
    gas_ = mother_logic_->GetMaterial();
    pressure_ =    gas_->GetPressure();
    temperature_ = gas_->GetTemperature();
  }



  void NextDemoEnergyPlane::Construct()
  {
    GeneratePositions();

    ///////////////////////////
    /////   Carrier Plate   ///

    G4Tubs* carrier_plate_nh_solid = 
      new G4Tubs("CARRIER_PLATE_NH", 0., carrier_plate_diam_/2., carrier_plate_thickness_/2., 0., twopi);

    // Making central hole for vacuum manifold
    G4Tubs* carrier_plate_central_hole_solid = 
      new G4Tubs("CARRIER_PLATE_CENTRAL_HOLE", 0., carrier_plate_central_hole_diam_/2., (carrier_plate_thickness_+1.*cm)/2., 0., twopi);
  
    G4SubtractionSolid* carrier_plate_solid = new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_nh_solid,
								     carrier_plate_central_hole_solid, 0, G4ThreeVector(0. , 0., 0.) );

    // Making PMT holes
    G4Tubs* carrier_plate_pmt_hole_solid = 
      new G4Tubs("CARRIER_PLATE_PMT_HOLE", 0., enclosure_diam_/2., (carrier_plate_thickness_+1.*cm)/2., 0., twopi);

    for (int i=0; i<num_PMTs_; i++) {
      carrier_plate_solid = new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_solid,
						   carrier_plate_pmt_hole_solid, 0, pmt_positions_[i]);
    }

    G4LogicalVolume* carrier_plate_logic = new G4LogicalVolume(carrier_plate_solid,
							       G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							       "CARRIER_PLATE");

    G4double carrier_plate_posz = energy_plane_posz_ - carrier_plate_thickness_/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,carrier_plate_posz), carrier_plate_logic,
		      "CARRIER_PLATE", mother_logic_, false, 0, false);
    

    ////////////////////////
    /////   Enclosures   ///

    /// Assign optical properties to materials ///
    G4Material* sapphire = MaterialsList::Sapphire();
    sapphire->SetMaterialPropertiesTable(OpticalMaterialProperties::Sapphire());
    G4Material* vacuum = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    vacuum->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());
    G4Material* optical_coupler = MaterialsList::OpticalSilicone();
    optical_coupler->SetMaterialPropertiesTable(OpticalMaterialProperties::OptCoupler());

    // // A "pseudo-enclosure"of vacuum is constructed to hold the elements that are replicated
    G4Tubs* enclosure_gas_solid = new G4Tubs("ENCLOSURE_GAS", 0., enclosure_diam_/2., enclosure_length_/2., 0., twopi);
    G4LogicalVolume* enclosure_gas_logic = 
      new G4LogicalVolume(enclosure_gas_solid, vacuum, "ENCLOSURE_GAS");
    
    G4Tubs* enclosure_flange_solid = 
      new G4Tubs("ENCLOSURE_FLANGE", enclosure_window_diam_/2., enclosure_diam_/2., enclosure_flange_length_/2., 0., twopi);

    G4LogicalVolume* enclosure_flange_logic =
      new G4LogicalVolume(enclosure_flange_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
			  "ENCLOSURE_FLANGE");
    
    G4ThreeVector flange_pos(0., 0., enclosure_length_/2. - enclosure_flange_length_/2.);
    new G4PVPlacement(0, flange_pos, enclosure_flange_logic,
      		      "ENCLOSURE_FLANGE", enclosure_gas_logic, false, 0, false);
  
   
    // Adding the sapphire window
    G4Tubs* enclosure_window_solid = 
      new G4Tubs("ENCLOSURE_WINDOW", 0.,enclosure_window_diam_/2., enclosure_window_thickness_/2., 0., twopi);

    G4LogicalVolume* enclosure_window_logic = 
      new G4LogicalVolume(enclosure_window_solid, sapphire, "ENCLOSURE_WINDOW");

    G4double window_posz =  enclosure_length_/2.-enclosure_window_thickness_/2. ;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,window_posz), enclosure_window_logic,
		      "ENCLOSURE_WINDOW", enclosure_gas_logic, false, 0, false);
    
    // Adding the optical pad
    G4Tubs* enclosure_pad_solid = 
      new G4Tubs("ENCLOSURE_PAD", 0., enclosure_window_diam_/2., enclosure_pad_thickness_/2., 0., twopi);

    // G4LogicalVolume* enclosure_pad_logic =
    //   new G4LogicalVolume(enclosure_pad_solid, MaterialsList::OpticalSilicone(),
    // 							       "ENCLOSURE_PAD");
    G4LogicalVolume* enclosure_pad_logic =
      new G4LogicalVolume(enclosure_pad_solid, optical_coupler, "ENCLOSURE_PAD");

    G4double pad_posz = window_posz - enclosure_window_thickness_/2. - enclosure_pad_thickness_/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pad_posz), enclosure_pad_logic,
		      "ENCLOSURE_PAD", enclosure_gas_logic, false, 0, false);

    /// TPB coating on sapphire window
    G4Material* tpb = MaterialsList::TPB();
    tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
    //tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB(_pressure, _temperature));
    // G4cout << "P and T on sapphire windows TPB: " << _pressure << 
    //   ", " << _temperature << G4endl;
    G4Tubs* tpb_solid = new G4Tubs("ENCLOSURE_TPB", 0., enclosure_window_diam_/2, 
				   tpb_thickness_/2., 0., twopi);
    G4LogicalVolume* tpb_logic = 
      new G4LogicalVolume(tpb_solid, tpb, "ENCLOSURE_TPB");

      

    // Adding the PMT
    pmt_->Construct();
    G4LogicalVolume* pmt_logic = pmt_->GetLogicalVolume();
    G4double pmt_rel_posz = pmt_->GetRelPosition().z();
    pmt_zpos_ = pad_posz - enclosure_pad_thickness_/2. - pmt_rel_posz;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pmt_zpos_), pmt_logic,
		      "PMT", enclosure_gas_logic, false, 0, true);  // false
    G4VisAttributes * vis3 = new G4VisAttributes;
    vis3->SetColor(0.5, 0.5, .5);
    vis3->SetForceSolid(true);
    pmt_logic->SetVisAttributes(vis3);
    
    // Adding the PMT base
    G4Tubs* pmt_base_solid = 
      new G4Tubs("PMT_BASE", 0., pmt_base_diam_/2., pmt_base_thickness_, 0.,twopi);
    G4LogicalVolume* pmt_base_logic = 
      new G4LogicalVolume(pmt_base_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"),
			  "PMT_BASE"); 
   
    G4double pmt_base_pos = -enclosure_length_/2. + pmt_base_thickness_; //to be checked
    new G4PVPlacement(0, G4ThreeVector(0.,0., pmt_base_pos),
		      pmt_base_logic, "PMT_BASE", enclosure_gas_logic, false, 0, false);

   
    
    // Placing the "pseudo-enclosures" with all internal components in place + TPB in front
    G4double enclosure_posz =  energy_plane_posz_ - enclosure_length_/2.;

   if (verbosity_) {
          G4cout << " *********  PMT  POSITION   PRINTING  ********* " << G4endl;
    G4cout << "EnclosureEndsZ = " 
           << enclosure_posz << " pls " << enclosure_length_/2. << " EQ " 
           <<  enclosure_posz + enclosure_length_/2.<< G4endl;
    // G4cout << "Enclosure ends at z = " << enclosure_posz + _enclosure_length/2. << G4endl;
    // G4cout << "TPB ends at z = " << _energy_plane_posz + _tpb_thickness << G4endl;
   }
    G4ThreeVector pos;
    G4ThreeVector tpb_pos;
    for (int i=0; i<num_PMTs_; i++) {
      pos = pmt_positions_[i];
      if (verbosity_) {
          G4cout << "Enclosure Pos XY = " << pos << G4endl;}
      pos.setZ(enclosure_posz);
      tpb_pos = pmt_positions_[i];
      tpb_pos.setZ(energy_plane_posz_ + tpb_thickness_/2.);      
      new G4PVPlacement(0, pos, enclosure_gas_logic,
			"PSEUDO-ENCLOSURE", mother_logic_, false, i, false);
      new G4PVPlacement(0, tpb_pos, tpb_logic, "ENCLOSURE_TPB", mother_logic_, 
			false, i, false);
    }

   if (verbosity_) {
    G4cout << " *******  END   PMT  POSITION   PRINTING  END  ******* " << G4endl;
   }
    // G4PVPlacement* enclosure_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), enclosure_logic,
    // 						     "ENCLOSURE", _mother_logic, false, 0);


    /////////////////////////////////////
    //  SETTING VISIBILITIES   //////////
    enclosure_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
    if (visibility_) {
      G4VisAttributes gas = nexus::Red();
      enclosure_gas_logic->SetVisAttributes(gas);
      G4VisAttributes copper_col = CopperBrown();
      // copper_col.SetForceSolid(true);
      carrier_plate_logic->SetVisAttributes(copper_col);
      G4VisAttributes enclosure_col = nexus::Brown();
      // enclosure_col.SetForceSolid(true);
      enclosure_flange_logic->SetVisAttributes(enclosure_col); 
      G4VisAttributes sapphire_col = nexus::Lilla();
      sapphire_col.SetForceSolid(true);
      enclosure_window_logic->SetVisAttributes(sapphire_col);
      G4VisAttributes pad_col = nexus::LightGreen();
      pad_col.SetForceSolid(true);
      enclosure_pad_logic->SetVisAttributes(pad_col);
      G4VisAttributes base_col = nexus::Yellow();
      base_col.SetForceSolid(true);
      pmt_base_logic->SetVisAttributes(base_col);
      G4VisAttributes tpb_col = nexus::LightBlue();
      tpb_col.SetForceSolid(true);
      tpb_logic->SetVisAttributes(tpb_col);     
    } else {
      carrier_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_flange_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_window_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_pad_logic->SetVisAttributes(G4VisAttributes::Invisible);
      pmt_base_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }


  NextDemoEnergyPlane::~NextDemoEnergyPlane()
  {
  }


  void NextDemoEnergyPlane::GeneratePositions()
  {
    /// Function that computes and stores the XY positions of PMTs in the carrier plate

    G4int total_positions = 0;
    G4ThreeVector position(0.,0.,0.);


    G4double rad = 7 * cm;   // Ruty: to be check !
    G4int num_places = 3;
    G4int step_deg = 360.0 / num_places;
    for (G4int place=0; place<num_places; place++) {
	G4double angle = place * step_deg + 36. ;
	position.setX(rad * sin(angle * deg));
	position.setY(rad * cos(angle * deg));
	pmt_positions_.push_back(position);
	total_positions++;
        //G4cout << "\n\ntotal_positions= ", total_positions;
    }

    //}

    // Checking
    if (total_positions != num_PMTs_) {
      G4cout << "\n\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
      exit(0);
    }

  }


}


