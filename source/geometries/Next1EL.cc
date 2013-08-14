// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>
//  Created: 27 Jan 2010
//  
//  Copyright (c) 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next1EL.h"

#include "PmtR7378A.h"
#include "SiPM11.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "HexagonNavigator.h"
#include "UniformElectricDriftField.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4Region.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UserLimits.hh>

#include <iostream>


namespace nexus {


  Next1EL::Next1EL(): BaseGeometry()
  {
    _PMTTable.open("data/PMTTable.dat",std::ios::trunc);
    _SiPMTable.open("data/SiPMTable.dat",std::ios::trunc);

    _PMTTable.close();
    _SiPMTable.close();

    _PMTID = 0;
    _SiPMID = 0;
    
    DefineGeometry();
  }
  
  
  
  Next1EL::~Next1EL()
  {
    delete _hexnav;
  }
  
  
  
  void Next1EL::DefineGeometry()
  {
    // Materials
    
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* silica = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    
    
    // LAB -----------------------------------------------------------
    // This is just a volume of air surrounding the detector so that
    // events (for instance, from calibration sources or cosmic rays)
    // can be generated on the outside.
    
    G4double lab_size = 2. * m; // big enough to fit the detector inside

    G4Box* lab_solid = 
      new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

    G4Material* air = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

    G4LogicalVolume* lab_logic = new G4LogicalVolume(lab_solid, air, "LAB");
    lab_logic->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic);
    
    
    // VESSEL --------------------------------------------------------
    // Stainless-steel cylindric vessel. Its dimensions are highlighted
    // below. Notice that the thickness of tube and endcaps is assumed 
    // equal, although in reality these are thicker.

    ////////////////////////////////////
    G4double vessel_diam   = 300. * mm;
    G4double vessel_length = 600. * mm;
    G4double vessel_thickn =   3. * mm;
    ////////////////////////////////////
    
    G4Tubs* vessel_solid = 
      new G4Tubs("VESSEL", 0., (vessel_diam/2. + vessel_thickn), 
		 (vessel_length/2. + vessel_thickn), 0., twopi);
    
    G4Material* steel = MaterialsList::Steel();

    G4LogicalVolume* vessel_logic = 
      new G4LogicalVolume(vessel_solid, steel, "VESSEL");
    
    G4PVPlacement* vessel_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_logic,
			"VESSEL", lab_logic, false, 0);
    
    
    // GAS -----------------------------------------------------------
    // Volume of gas (typically Xenon) filling the vessel.

    G4Tubs* gas_solid = 
      new G4Tubs("GAS", 0., vessel_diam/2., vessel_length/2., 0, twopi);
    
    // FIXME. Gas pressure should be read from configuration parameters
    G4Material* GXe = MaterialsList::GXe(10.*bar);
    GXe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe());
    
    G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, GXe, "GAS");
    
    G4PVPlacement* gas_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic,
			"GAS", vessel_logic, false, 0);
    
    
    // LIGHT TUBE ----------------------------------------------------
    // Hexagonal prism of teflon used as light tube (it transports the
    // EL light from anode to cathode improving the light collection
    // efficiency of the energy plane).
    
    ////////////////////////////////////
    G4double tube_diam   = 190.  * mm;
    G4double tube_length = 400.  * mm;
    G4double tube_thickn =   0.1 * mm;
    ////////////////////////////////////

    _radius = tube_diam/2.;
    _length = tube_length/2.;

    _hexnav = new HexagonNavigator(_radius,_length);

    // Position of the Z planes of the light tube
    G4double zplane[] = {-tube_length/2., tube_length/2.};
    // Inner radius of the tube
    G4double rinner[] = {tube_diam/2., tube_diam/2.};
    // Outer radius of the tube
    G4double router[] = {(tube_diam/2. + tube_thickn),
			 (tube_diam/2. + tube_thickn)};

    G4Polyhedra* tube_solid =
      new G4Polyhedra("LIGHT_TUBE", 0., twopi, 6, 2, zplane, rinner, router);
    
    G4Material* teflon = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
    
    G4LogicalVolume* tube_logic = 
      new G4LogicalVolume(tube_solid, teflon, "LIGHT_TUBE");
    
    G4PVPlacement* tube_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), tube_logic, 
			"LIGHT_TUBE", gas_logic, false, 0);
    
    
    
    _PMTTable.open("data/PMTTable.dat",std::ios::app);
    _SiPMTable.open("data/SiPMTable.dat",std::ios::app);
  
    
    // ACTIVE --------------------------------------------------------
    // Active volume of the detector.

    ////////////////////////////////////
    G4double active_length = 300. * mm;
    G4double active_diam   = tube_diam;
    ////////////////////////////////////

    // Position of the Z planes of active
    zplane[0] = -active_length/2.; zplane[1] = active_length/2.;
    // 
    rinner[0] = 0.; rinner[1] = 0.;
    router[0] = active_diam/2.; router[1] = active_diam/2.; 

    G4Polyhedra* active_solid =
      new G4Polyhedra("ACTIVE", 0., twopi, 6, 2, zplane, rinner, router);

    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, GXe, "ACTIVE");
    
    G4PVPlacement* active_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic, 
			"ACTIVE", gas_logic, false, 0);
    
    
    // Limit the step size in this volume for better tracking precision
    active_logic->SetUserLimits(new G4UserLimits(1.*mm));
    
    // Set the volume as an ionization sensitive detector
    IonizationSD* ionisd = new IonizationSD("/NEXT1/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);
    
    // Define a drift field for this volume
    UniformElectricDriftField* field = new UniformElectricDriftField();
    field->SetCathodePosition(zplane[0]);
    field->SetAnodePosition(zplane[1]);
    field->SetDriftVelocity(1.*mm/microsecond);
    field->SetTransverseDiffusion(1.*mm/sqrt(cm));
    field->SetLongitudinalDiffusion(1.*mm/sqrt(cm));
    field->SetMaterial(GXe);

    G4Region* drift_region = new G4Region("DRIFT");
    drift_region->SetUserInformation(field);
    drift_region->AddRootLogicalVolume(active_logic);
    


        
    // ELECTROLUMINESCENCE GAP ---------------------------------------
    //

    ////////////////////////////////////
    G4double gap_length = 10. * mm;
    ////////////////////////////////////

    zplane[0] = active_length/2.; zplane[1] = active_length/2. + gap_length;
    
    G4Polyhedra* gap_solid =
      new G4Polyhedra("EL_GAP", 0., twopi, 6, 2, zplane, rinner, router);

    G4LogicalVolume* gap_logic = new G4LogicalVolume(gap_solid, GXe, "EL_GAP");
    
    G4PVPlacement* gap_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gap_logic, 
			"EL_GAP", gas_logic, false, 0);
    
    
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    el_field->SetCathodePosition(zplane[0]);
    el_field->SetAnodePosition(zplane[1]);
    el_field->SetDriftVelocity(5.*mm/microsecond);
    el_field->SetFieldStrength(20*kilovolt);
    el_field->SetMaterial(GXe);
    // el_field->SetTransverseDiffusion(1.*mm/sqrt(cm));
    // el_field->SetLongitudinalDiffusion(1.*mm/sqrt(cm));

    G4Region* el_region = new G4Region("EL_REGION");
    el_region->SetUserInformation(el_field);
    el_region->AddRootLogicalVolume(gap_logic);

    
    
    // TRACKING PLANE REFLECTOR --------------------------------------
    //

    ////////////////////////////////////
    G4double refl_diam    =  175. * mm;
    G4double refl_thick   =    1. * mm;
    G4double refl_dist_el =    5. * mm;
    ////////////////////////////////////

    zplane[0] = active_length/2. + gap_length + refl_dist_el;
    zplane[1] = zplane[0] + refl_thick; 
    router[0] = refl_diam/2.; router[1] = refl_diam/2.;

    G4Polyhedra* reflector_solid =
      new G4Polyhedra("TRACKING_REFL", 0., twopi, 6, 2, zplane, rinner, router);
   
    G4LogicalVolume* reflector_logic =
      new G4LogicalVolume(reflector_solid, teflon, "TRACKING_REFL");

    G4PVPlacement* reflector_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), reflector_logic,
			"TRACKING_REFL", gas_logic, false, 0);


    SiPM11* sipm = new SiPM11();
    G4LogicalVolume* sipm_logic = sipm->GetLogicalVolume();

    G4PVPlacement* sipm_physi;

    G4double x, y;
    G4int copy_no = 0;

    std::vector<G4ThreeVector> SiPM_positions;
    G4double rmin = 1.*cm;
    G4double SiPM_z = 163.*mm;

    SiPM_positions = _hexnav->SetDetector(rmin,_radius, SiPM_z);

    for (G4int i=0; i<SiPM_positions.size(); i++) {

      sipm_physi = 
	new G4PVPlacement(0, SiPM_positions[i], sipm_logic,
			  "SILICON_PM", gas_logic, false, copy_no);
	
      _SiPMTable<<_SiPMID<<" "<<SiPM_positions[i].x()<<" "<<SiPM_positions[i].y()<<" "<<SiPM_positions[i].z()<<"\n";
	
      copy_no++;
      _SiPMID++;
    }

    _SiPMTable.close();
    




    
    // PMTs

    PmtR7378A* pmt = new PmtR7378A();
    G4LogicalVolume* pmt_logic = pmt->GetLogicalVolume();

    G4double Radius = (175./2) * mm;
    G4double apothem = (3./16) * Radius;
    G4double radius = 2/sqrt(3) * apothem;

    // Temporarily I'll place the pmts by hand

    G4PVPlacement* pmt_physi;


    std::vector<G4ThreeVector> PMT_positions;
    G4double PMT_r = 2.54/2.*cm;
    G4double PMT_z = -175.*mm;
    G4int PMT_copy_no = 0;

    PMT_positions = _hexnav->SetDetector(PMT_r,_radius, PMT_z,false);

    for (G4int j=0; j<PMT_positions.size(); j++) {

      pmt_physi = 
	new G4PVPlacement(0, PMT_positions[j], pmt_logic,
			  "PMT", gas_logic, false, PMT_copy_no);
	
      _PMTTable<<_PMTID<<" "<<PMT_positions[j].x()<<" "<<PMT_positions[j].y()<<" "<<PMT_positions[j].z()<<"\n";

      PMT_copy_no++;
      _PMTID++;
    }


    _PMTTable.close();

    // OPTICAL SURFACES

    G4double energies[2] = {4.0*eV, 8.520*eV};
    G4double reflectivity[2] = {1., 1.};
    G4double efficiency[2] = {0., 0.};

    G4MaterialPropertiesTable* teflon_mt = new G4MaterialPropertiesTable();
    teflon_mt->AddProperty("EFFICIENCY", energies, efficiency, 2);
    teflon_mt->AddProperty("REFLECTIVITY", energies, reflectivity, 2);

    G4OpticalSurface* opsurf_tube = 
      new G4OpticalSurface("LIGHT_TUBE", unified, ground, dielectric_metal);
    opsurf_tube->SetSigmaAlpha(2.*pi);
    opsurf_tube->SetMaterialPropertiesTable(teflon_mt);
    
    new G4LogicalSkinSurface("LIGHT_TUBE", tube_logic, opsurf_tube);
    //new G4LogicalSkinSurface("LIGHT_TUBE", reflector_logic, opsurf_tube);
  }



  G4ThreeVector Next1EL::GenerateVertex(const G4String& region) const
  {
    if (region == "ANODE") 
      return G4ThreeVector();
    else if (region == "CATHODE")
      return G4ThreeVector();
    else
      return G4ThreeVector(0., 0., 0.);
      //return _hexnav->GenerateVertex(region);
  }
  
  
} // end namespace nexus
