// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: Daniel P. Hogan <dphogan@berkeley.edu>
//           F. Monrabal <francesc.monrabal@ific.uv.es>
//  Created: 1 December 2010
//  
//  Copyright (c) 2010, 2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next1Lbnl.h"

#include "ConfigService.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "HexagonPointSampler.h"
#include "PmtR7378A.h"
#include "SiPM11.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>
#include <G4Ellipsoid.hh>
#include <G4UnionSolid.hh>
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
#include <G4RunManager.hh>


namespace nexus {

  Next1Lbnl::Next1Lbnl(): BaseGeometry()
  {

    // From Next1EL.cc: for EL Table generation
    _idx_table = 0;
    _table_vertices.clear();

    // Read the parameters from the configuration file
    ReadParameters();

    // Build the geometry, and calculate the EL table position locations
    BuildGeometry();

  }
  
  Next1Lbnl::~Next1Lbnl() {}

  void Next1Lbnl::ReadParameters()
  {
    const ParamStore& cfg = ConfigService::Instance().Geometry();

    // Binning
    if(cfg.PeekDParam("EL_binning"))
      _binning = cfg.GetDParam("EL_binning");
    else
      _binning = 1. * cm;

    // Enable SiPM tracking plane
    _tracking_plane = cfg.GetIParam("tracking_plane");

    // Gas pressure
    _pressure = cfg.GetDParam("pressure");
  
    // Maximum step size
    if (cfg.PeekDParam("max_step_size"))
      _max_step_size = cfg.GetDParam("max_step_size");
    else
      _max_step_size = 1. * mm;
    G4cout << "Maximum step size is set to " << _max_step_size << "\n";

    // Number of events
    const ParamStore& cfg_job = ConfigService::Instance().Job();
    _numb_of_events = cfg_job.GetIParam("number_events");

    // EL field on/off
    G4int _el_on = 0;
    if(cfg.PeekIParam("el_field")) _el_on = cfg.GetIParam("el_field");

}

  
  void Next1Lbnl::BuildGeometry()
  {

    // DIMENSIONS //////////////////////////////////////////////////////////////

    //Used when array of material properties called for
    //but I only have one value.  This is a hack.
    G4double arbe1 = 0.1*eV;
    G4double arbe2 = 100*eV;

    //Used to fix overlap & adjacent surface problems.
    //Treat as infinitesimally small.
    G4double epsilon = .10*mm;

    //Identity matrix
    G4RotationMatrix * id = new G4RotationMatrix;

    G4double halfinch = 1.27*cm; // do not use in geometric object declarations!

    G4double lodim = 11.*cm; // pressure vessel outer radius
    G4double lidim = lodim - halfinch; // pressure vessel inner radius
    G4double lsmex = 29.12*cm; // "SMooth EXtent" -- axis of cylindrical vessel part
    G4double lenddep = 6.65*cm; // curved end of vessel axis length
    G4double lrimth = 2.38*cm; // thickness of rim
    G4double lrimax = 3.19*cm; // length of axis of rim cylinder
    G4double llidax = 3.0*cm; // lid thickness (axis length)
    G4double lxemex = lsmex - halfinch; // axis of cylindrical part of xenon cavity
    G4double lxenddep = lenddep - halfinch; // curved end of xenon cavity length
    G4double lphr = 1.27*cm; // radius of phototube
    G4double lphax = 4.5*cm; // axial length of phototube (excluding pins)
    G4double lphz = -((lsmex-lphax)/2.-2.05*cm); // phototube z-position
    G4double lphspac = (12.38*cm-2.0*lphr)/(2.0*sqrt(3)); // phototube spacing
    G4double ltrthick = 0.6*cm;  // teflon reflector thickness
    G4double ltrwide = 7.12*cm;  // teflon reflector width
    G4double ltrlong = 16.89*cm; // teflon reflector length
    G4double ELPMdist = 5.*mm;
    G4double ltred = 7.54*cm; // end of reflector to end of cylinder
    G4double ltrcent = -lxemex/2.0+ltred+ltrlong/2.0; // z-coord of reflector center (uncut)
    G4double ltrback = -lxemex/2.0+ltred; //z coord of reflector back face (closer to region 1)
    G4double ltrcutlong = ltrlong/2+61.15*mm-ltrcent-2.5*mm+ELPMdist; //teflon reflector length when cut for SiPM.
      //61.15=zpos of end of EL in cavity_log coord. 2.5=dbothickness
    G4double lfro = 6.82*cm; // distance to bottom edge of field region 1
    G4double lfr1 = 4.96*cm; // axial length of field region 1
    G4double lfr2 = 7.96*cm; // axial length of drift region
    G4double lfr3 = 0.30*cm; // axial length of electroluminescent region
    G4double lfr4 = 4.96*cm; // axial length of field region 4
    G4double lfrrad = ltrwide*(sqrt(3)/2.0); // center-to-edge distance of field regions
    G4double lbrrad = 2.68*2.54*cm; // center to edge of back reflector
    G4double lbrax = 0.25*2.54*cm; // thickness of back reflector

    //Some electric field strengths: +z (towards door) component
    G4double Efield1z =  19.0*kilovolt / lfr1; //near phototubes
    G4double Efield2z = -31.0*kilovolt / lfr2; //drift region
    G4double Efield3z =  -9.0*kilovolt / lfr3; //electroluminescent region
    G4double Efield4z =  20.0*kilovolt / lfr4; //near door

    // EL TABLE ///////////////////////////////////////////////////////////////

    // Calculate EL table initial positions for ionielectrons (
    // (note: z is in absolute coordinates while many regions are positioned relative to cavity_log)
    G4double z = lsmex/2.0-lxemex+lfro+lfr1+lfr2;
    CalculateELTableVertices(ltrwide, lfrrad, _binning, z+0.5*mm);
    G4cout << "Table boundaries are +/-(xlen,ylen) = +/-(" << ltrwide << ", " << lfrrad << ") with binning " << _binning << " and z = " << (z+0.5*mm) << "\n";
    
    // MATERIALS ///////////////////////////////////////////////////////////////
    
    // Retrieve the NIST materials database
    G4NistManager* nistmanager = G4NistManager::Instance();

    // Air
    G4Material* air = nistmanager->FindOrBuildMaterial("G4_AIR");

    // Teflon
    G4Material * teflon = nistmanager->FindOrBuildMaterial("G4_TEFLON");

    // Quartz
    G4Material* quartz = nistmanager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    quartz->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());

    // Steel (approx values for density and carbon fraction)
    G4Material* steel = MaterialsList::Steel();

    // High-pressure xenon (HPXe)
    G4Material* HPXe = MaterialsList::GXe(_pressure, 293);
    HPXe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, 293));
    
    // Dielectric to simulate the opacity of the grids
    G4Material* dielectric_grid = MaterialsList::FakeDielectric(HPXe, "GRID");
    G4MaterialPropertiesTable* mpt_grid =
      OpticalMaterialProperties::FakeGrid(_pressure, 293, .88, 0.5*mm);
    dielectric_grid->SetMaterialPropertiesTable(mpt_grid);

    //G4Material* Dielectric_Xe = nistmanager->
    //ConstructNewGasMaterial("Dielectric_Xe","G4_Xe",STP_Temperature,10.*bar);
    //Dielectric_Xe->SetMaterialPropertiesTable(OpticalMaterialProperties::DielectricGrid());

    // EXPERIMENTAL HALL ///////////////////////////////////////////////////////

    G4double expHall_x = 1.0*m;
    G4double expHall_y = 1.0*m;
    G4double expHall_z = 1.0*m;

    G4Box* experimentalHall_box
      = new G4Box("expHall_box",expHall_x/2.0,expHall_y/2.0,expHall_z/2.0);
    G4LogicalVolume* experimentalHall_log = 
      new G4LogicalVolume(experimentalHall_box, air,"expHall_log",0,0,0);
    //Make experimental hall invisible.
    experimentalHall_log->SetVisAttributes(G4VisAttributes::Invisible);
    
    this->SetLogicalVolume(experimentalHall_log);
    
    // PRESSURE VESSEL /////////////////////////////////////////////////////////

    // Pressure vessel body
    G4VSolid * vesselbody_shape = 
      new G4Tubs("vesselbody_shape", 0*cm, lodim, lsmex/2.0, 0, 2*pi);

    // Pressure vessel curved end
    G4VSolid * vesselend_shape = 
      new G4Ellipsoid("vesselend_shape", lodim, lodim, lenddep, -lenddep, 0*cm);

    // Pressure vessel rim
    G4VSolid * vesselrim_shape = 
      new G4Tubs("vesselrim_shape", lodim-epsilon, lodim+lrimth, lrimax/2.0, 0, 2*pi);

    // Union of body, end, and rim
    G4ThreeVector vesselend_trans(0,0,-lsmex/2.0);
    G4ThreeVector vesselrim_trans(0,0,(lsmex-lrimax)/2.0);
    G4UnionSolid * vesselmid_shape = 
      new G4UnionSolid("vesselmid_shape",vesselbody_shape,
		       vesselrim_shape,id,vesselrim_trans);
    G4UnionSolid * vesselmain_shape = 
      new G4UnionSolid("vesselmain_shape",vesselmid_shape,
		       vesselend_shape,id,vesselend_trans);

    // Logical and physical vessel
    G4LogicalVolume * vesselmain_log = 
      new G4LogicalVolume(vesselmain_shape,steel,"vesselmain_log");
    G4VPhysicalVolume * vesselmain_phys = 
      new G4PVPlacement(0,G4ThreeVector(),vesselmain_log,"vesselmain",
			experimentalHall_log,false,0);

    // Lid
    G4VSolid* lid_shape = 
      new G4Tubs("lid_shape",0*cm, lodim+lrimth, llidax/2.0, 0,2*pi);
    G4LogicalVolume* lid_log = 
      new G4LogicalVolume(lid_shape, steel, "lid_log");
    G4VPhysicalVolume* lid_phys = 
      new G4PVPlacement(0,G4ThreeVector(0,0,(lsmex+llidax)/2.0),lid_log,
			"lid",experimentalHall_log,false,0);

    // Xenon-filled cavity
    G4VSolid* cavitycyl_shape = 
      new G4Tubs("cavitycyl_shape", 0*cm, lidim, lxemex/2.0, 0, 2*pi);
    G4VSolid* cavityel_shape = 
      new G4Ellipsoid("cavityel_shape", lidim, lidim, lxenddep, -lxenddep, 0*cm);
    G4ThreeVector cavityel_trans(0,0,-lxemex/2.0);
    G4VSolid* cavity_shape = 
      new G4UnionSolid("cavity_shape", cavitycyl_shape, 
		       cavityel_shape, id, cavityel_trans);
    G4LogicalVolume* cavity_log = 
      new G4LogicalVolume(cavity_shape, HPXe, "cavity_log");
    G4PVPlacement* cavity_phys = 
      new G4PVPlacement(0,G4ThreeVector(0,0,(lsmex-lxemex)/2.0),
			cavity_log,"cavity",vesselmain_log,false,0);
    //In previous line, first 0 is rotation; last 0 is copy #
    //changed - to + in G4PVPlacement * cavity_phys ...
 
    // PHOTOMULTIPLIERS ////////////////////////////////////////////////////////

    //Gets logical volume for Nexus phototube geometry
    PmtR7378A pmt;
    G4LogicalVolume* pmt_logic = pmt.GetLogicalVolume();
    G4PVPlacement* phototube_phys;
    G4int cx, cy, cn=0; //counters for x-axis, y-axis, total # of phototubes
    for (cy=-2; cy<=2; cy++) {
      for (cx=-4+abs(cy); cx<=4-abs(cy); cx+=2) {
	
	    G4double xpos = lphspac*cx/2.0;
	    G4double ypos = lphspac*sqrt(3)/2.0*cy;
	    //NEXUS SPECIFIC
	    phototube_phys = 
	      new G4PVPlacement(0,G4ThreeVector(xpos,ypos,lphz),
			    pmt_logic,"phototube",cavity_log,false,cn,true);
	    cn++;
      }
    }

      
    //create null logical volumes
    G4LogicalVolume* reflector_log=0;
    G4LogicalVolume* backref_log=0;


    // TRACKING PLANE //////////////////////////////////////////////////////////
    
    if (_tracking_plane) {
        
        DefineDaughterBoards();
        
        G4RotationMatrix rotdbo;
        rotdbo.rotateX(pi);
        G4RotationMatrix rotcbupleft;   //same as down right, facing in -Z direction, use corner board with coating in +Z
        G4RotationMatrix rotcbdownright;
        G4RotationMatrix rotcbupright;  //same as down left, facing in -Z direction, use corner board with coating in -Z
        G4RotationMatrix rotcbdownleft;
        /////fix nexus overlap issue with region 4 by placing DBO44 inside region4_log instead
        //of cavity_log
        //G4double pos_z = 2.*mm;
        G4double pos_z = -lxemex/2. + lfro + lfr1 + lfr2 + lfr3 + ELPMdist;
        //5mm is distance from second grid to middle of daughter board - is variable
        //daughter boards are 5.001mm thick
        
        new G4PVPlacement(G4Transform3D(rotdbo, G4ThreeVector(0.,0.,pos_z)), 
                        _dbo44_logic, "DBO44", cavity_log, false, 0, true);
        new G4PVPlacement(G4Transform3D(rotdbo, G4ThreeVector(0.,4.*cm,pos_z)), 
                        _dbo44_logic, "DBO44", cavity_log, false, 1, true);
        new G4PVPlacement(G4Transform3D(rotdbo, G4ThreeVector(0.,-4.*cm,pos_z)), 
                        _dbo44_logic, "DBO44", cavity_log, false, 2, true);
        new G4PVPlacement(G4Transform3D(rotdbo, G4ThreeVector(4.*cm,0.,pos_z)), 
                        _dbo44_logic, "DBO44", cavity_log, false, 3, true);
        new G4PVPlacement(G4Transform3D(rotdbo, G4ThreeVector(-4.*cm,0.,pos_z)), 
                        _dbo44_logic, "DBO44", cavity_log, false, 4, true);
        //Start putting in corner pieces
        new G4PVPlacement(G4Transform3D(rotcbupleft.rotateX(pi).rotateZ(-pi/2), G4ThreeVector(-3.*cm,4*cm,pos_z)),
                          _dboCU_logic, "CBOposz", cavity_log, false, 0, true);
        new G4PVPlacement(G4Transform3D(rotcbdownright.rotateX(pi).rotateZ(pi/2), G4ThreeVector(3.*cm,-4.*cm,pos_z)),
                          _dboCU_logic, "CBOposz", cavity_log, false, 1, true);
        new G4PVPlacement(G4Transform3D(rotcbupright.rotateZ(-pi/2), G4ThreeVector(3.*cm,4.*cm,pos_z)),
                          _dboCD_logic, "CBOnegz", cavity_log, false, 0, true);
        new G4PVPlacement(G4Transform3D(rotcbdownleft.rotateZ(pi/2), G4ThreeVector(-3.*cm, -4.*cm, pos_z)),
                          _dboCD_logic, "CBOnegz", cavity_log, false, 1, true);
        
          
        //Teflon walls need to be cut for overlaps if SiPMs are turned on.
        // SIDE REFLECTORS /////////////////////////////////////////////////////////
          
        G4double zplane[2] = {0, ltrcutlong};
        G4double rinner[2] = {lfrrad, lfrrad};
        G4double router[2] = {rinner[0]+ltrthick/2., rinner[0]+ltrthick/2.};
          
        G4Polyhedra* reflector_shape =
        new G4Polyhedra("reflector_shape", 0., twopi, 6, 2, zplane, rinner, router);
        reflector_log = 
        new G4LogicalVolume(reflector_shape, teflon, "reflector_log");
        G4PVPlacement* reflector_phys =
        new G4PVPlacement(0, G4ThreeVector(0.,0.,ltrback), reflector_log, 
                        "reflector", cavity_log, false, 0, true);
          
        // BACK REFLECTOR //////////////////////////////////////////////////////////
          
        G4double backrefz[2] = {0, lbrax};
        G4double backrefi[2] = {0, 0};
        G4double backrefr[2] = {lbrrad, lbrrad};
        
        G4VSolid* backref_shape = 
        new G4Polyhedra("backref_shape", 0, 2*pi, 6, 2, backrefz, backrefi, backrefr);
        backref_log =
        new G4LogicalVolume(backref_shape, teflon, "backref_log");
        G4PVPlacement* backref_phys =
        new G4PVPlacement(0, G4ThreeVector(0,0,-lxemex/2.0+lfro+lfr1+lfr2+lfr3+lfr4), 
                        backref_log, "backref", cavity_log, false, 0, true);
    }

    else{  //no SiPMs
          
        // SIDE REFLECTORS /////////////////////////////////////////////////////////
          
        G4double zplane[2] = {-ltrlong/2., ltrlong/2.};
        G4double rinner[2] = {lfrrad, lfrrad};
        G4double router[2] = {rinner[0]+ltrthick/2., rinner[0]+ltrthick/2.};
        
        G4Polyhedra* reflector_shape =
        new G4Polyhedra("reflector_shape", 0., twopi, 6, 2, zplane, rinner, router);
        reflector_log = 
        new G4LogicalVolume(reflector_shape, teflon, "reflector_log");
        G4PVPlacement* reflector_phys =
        new G4PVPlacement(0, G4ThreeVector(0.,0.,ltrcent), reflector_log, 
                        "reflector", cavity_log, false, 0, true);
          
        // BACK REFLECTOR //////////////////////////////////////////////////////////
          
        G4double backrefz[2] = {0, lbrax};
        G4double backrefi[2] = {0, 0};
        G4double backrefr[2] = {lbrrad, lbrrad};
        
        G4VSolid* backref_shape = 
        new G4Polyhedra("backref_shape", 0, 2*pi, 6, 2, backrefz, backrefi, backrefr);
        backref_log =
        new G4LogicalVolume(backref_shape, teflon, "backref_log");
        G4PVPlacement* backref_phys =
        new G4PVPlacement(0, G4ThreeVector(0,0,-lxemex/2.0+lfro+lfr1+lfr2+lfr3+lfr4), 
                        backref_log, "backref", cavity_log, false, 0, true);
    }
      
      
    // Reflective surface for teflon
    G4OpticalSurface * teflonsurface = new G4OpticalSurface("teflonsurface");
    G4LogicalSkinSurface * teflonsurface_log = new G4LogicalSkinSurface("teflonsurface_log", reflector_log, teflonsurface);
    teflonsurface->SetType(dielectric_metal);

    //************ADDING NEW CODE FOR DIFFUSE REFLECTIVITY**********
    //Options are unified or GLISUR model.  Use unified.
    teflonsurface->SetModel(unified);
    teflonsurface->SetFinish(ground);
    teflonsurface->SetSigmaAlpha(0.1);//in radians
    //SigmaAlpha is determined by how bumpy the surface is.  Highly polished surface has sigmaalpha~0.3 degrees,
    //mechanically polished&chemically etched surface has sigmaalpha~4 degrees
    //Source: IEEE TRANSACTIONS ON NUCLEAR SCIENCE, VOL. 57, NO. 3, JUNE 2010
    //Simulating Scintillator Light Collection Using
    //Measured Optical Reflectance Martin Janecek, Member, IEEE, and William W. Moses, Fellow, IEEE

    //************END NEW CODE**************************************

    G4MaterialPropertiesTable * teflonsurface_table = new G4MaterialPropertiesTable();
    G4double energy[2]={arbe1, arbe2};      //Defined above
    G4double teflonreflectivity = 0.5;      //this is the total reflectivity R.  G4 first checks to see if photon is
    //reflected or absorbed.  if reflected, then goes to next 4 parameters
    G4double teflonreflect[2]={teflonreflectivity, teflonreflectivity};     // this says the total reflectivity is constant regardless of the photon energy, 
    // otherwise it will interpolate between 2 extremes
    G4double specularlobe[2]={0,0};         //this gives the percentage of reflected photons that neighbor the pure specular reflection
    G4double specularspike[2]={0,0};        //pure specular reflection percentage
    G4double backscatter[2]={0,0};      //backscatter percentage
    //pure Lambertian percentage is defined by previous 3 parameters as 1-(specularlobe+specularspike+backscatter)
    //all 4 parameters add up to 1.

    teflonsurface_table->AddProperty("REFLECTIVITY",energy,teflonreflect,2);//part of old code
    teflonsurface_table->AddProperty("SPECULARLOBECONSTANT",energy,specularlobe,2);
    teflonsurface_table->AddProperty("SPECULARSPIKECONSTANT",energy,specularspike,2);
    teflonsurface_table->AddProperty("BACKSCATTERCONSTANT",energy,backscatter,2);
      
    //********CONTINUE CODE HERE************************************
    teflonsurface->SetMaterialPropertiesTable(teflonsurface_table);
      
    //Add same surface to back reflector
    G4LogicalSkinSurface * teflonbacksurface_log = new G4LogicalSkinSurface("teflonbacksurface_log", backref_log, teflonsurface);
      

    //Set a step limit in the xenon
    //Must also add step limiting as a physics process in PhysicsList.cc
    G4UserLimits * maxStepLimitObject = new G4UserLimits(_max_step_size);

    // ELECTRIC FIELD VOLUMES ////////////////////////////////////////
    // NEXUS SPECIFIC NEXUS SPECIFIC NEXUS SPECIFIC

    // Used in all electric field sections
    G4double regionr[2] = {lfrrad,lfrrad};
    G4double regioni[2] = {0, 0};

    // Section 1 (E-field volume closest to 19 PMT's)
    G4double region1z[2] = {0,lfr1};
    G4VSolid * region1_shape = new G4Polyhedra("region1_shape", 0, 2*pi, 
     					       6, 2, region1z, regioni, regionr);
    G4LogicalVolume * region1_log = 
      new G4LogicalVolume(region1_shape, HPXe, "region1_log");
    G4PVPlacement * region1_phys = 
      new G4PVPlacement(0, G4ThreeVector(0,0,-lxemex/2.0+lfro),region1_log, 
			 "region1", cavity_log, false, 0,true);

    // Section 2 (Drift region)
    G4double region2z[2] = {0,lfr2};
    G4VSolid * region2_shape = new G4Polyhedra("region2_shape", 0, 2*pi, 
     					       6, 2, region2z, regioni, regionr);
    G4LogicalVolume * region2_log = 
      new G4LogicalVolume(region2_shape, HPXe, "region2_log");
    G4PVPlacement * region2_phys = 
      new G4PVPlacement(0, G4ThreeVector(0,0,-lxemex/2.0+lfro+lfr1),region2_log, 
			 "region2", cavity_log, false, 0,true);
    // Make volume an ionization sensitive detector
    IonizationSD * isd2 = new IonizationSD("/NEXTLBL/DRIFT");
    region2_log->SetSensitiveDetector(isd2);
    G4SDManager::GetSDMpointer()->AddNewDetector(isd2);
    // Set the maximum step length
    region2_log->SetUserLimits(maxStepLimitObject);

    // Section 3 (Electroluminescent region)
    G4double region3z[2] = {0,lfr3};
    G4VSolid * region3_shape = new G4Polyhedra("region3_shape", 0, 2*pi, 
     					       6, 2, region3z, regioni, regionr);
    G4LogicalVolume * region3_log = 
      new G4LogicalVolume(region3_shape, HPXe, "region3_log");
    G4PVPlacement * region3_phys = 
      new G4PVPlacement(0, G4ThreeVector(0,0,-lxemex/2.0+lfro+lfr1+lfr2),
			 region3_log, "region3", cavity_log, false, 0,true);
      
    G4cout<<"-lxemex/2+lfro+lfr1+lfr2="<<-lxemex/2.0+lfro+lfr1+lfr2<<G4endl;
    // Set the maximum step length
    region3_log->SetUserLimits(maxStepLimitObject);


    //Section 4 (E-field volume closest to chamber opening)
    G4double region4z[2] = {0,lfr4};
    G4VSolid * region4_shape = new G4Polyhedra("region4_shape", 0, 2*pi, 
     					       6, 2, region4z, regioni, regionr);
    G4LogicalVolume * region4_log = 
    new G4LogicalVolume(region4_shape, HPXe, "region4_log");
    G4PVPlacement * region4_phys = 
    new G4PVPlacement(0, G4ThreeVector(0,0,-lxemex/2.0+lfro+lfr1+lfr2+lfr3),
		     region4_log, "region4", cavity_log, false, 0,true);

    // Create the grids with separate logical volumes for grids in and out of the drift region
    G4double grid[2] = {0,0.5*mm};
    G4VSolid * grid_shape = new G4Polyhedra("grid_shape", 0, 2*pi, 
				           6, 2, grid, regioni, regionr);
    G4LogicalVolume * grid_log = 
      new G4LogicalVolume(grid_shape, dielectric_grid, "grid_log");
    G4LogicalVolume * grid_log_nondrift = 
      new G4LogicalVolume(grid_shape, dielectric_grid, "grid_log_nondrift");


    G4PVPlacement * grid1_phys = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0),
		     grid_log_nondrift, "grid1", region1_log, false, 0,true);
    G4PVPlacement * grid2_phys = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0),
		     grid_log, "grid2", region2_log, false, 0,true);
    G4PVPlacement * grid3_phys = 
      new G4PVPlacement(0, G4ThreeVector(0,0,lfr2-0.5*mm),
		     grid_log, "grid3", region2_log, false, 0,true);
    G4PVPlacement * grid4_phys = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0),
		     grid_log_nondrift, "grid4", region4_log, false, 0,true);
    G4PVPlacement * grid5_phys = 
      new G4PVPlacement(0, G4ThreeVector(0,0,lfr4-0.5*mm),
		     grid_log_nondrift, "grid5", region4_log, false, 0,true);


    //Make the specified volume always solid.
    //G4VisAttributes * visattribsolid = new G4VisAttributes;
    //visattribsolid->SetForceSolid(true);
    // grid_log->SetVisAttributes(visattribsolid);
    // region1_log->SetVisAttributes(visattribsolid);

    //Drift variables
    G4double difftrans = 1.*mm/sqrt(cm);
    G4double difflong = 1.*mm/sqrt(cm);

    G4double density = 5. * g/cm3;


    // Section 2: Drift Electric Field
    UniformElectricDriftField * field2 = new UniformElectricDriftField();
    field2->SetCathodePosition(-lxemex/2.+halfinch/2.+lfro+lfr1     );
    field2->SetAnodePosition(  -lxemex/2.+halfinch/2.+lfro+lfr1+lfr2);
    field2->SetDriftVelocity(1*mm/microsecond); //returndriftv(Efield2z, density));
    field2->SetTransverseDiffusion(difftrans);
    field2->SetLongitudinalDiffusion(difflong);
    //field2->SetMaterial(HPXe);
    G4Region * driftregion2 = new G4Region("driftregion2");
    driftregion2->SetUserInformation(field2);
    driftregion2->AddRootLogicalVolume(region2_log);



    G4cout << "Cathode position: " << (-lxemex/2.+halfinch/2.+lfro+lfr1)  << "\n";
    G4cout << "Anode position: " << (-lxemex/2.+halfinch/2.+lfro+lfr1+lfr2) << "\n";

    // Section 3: EL Electric Field
    UniformElectricDriftField * field3 = 0;
    G4Region * driftregion3 = 0;
    if(_el_on == 1) {
      field3 = new UniformElectricDriftField();
      field3->SetCathodePosition(-lxemex/2.+halfinch/2.+lfro+lfr1+lfr2     );
      field3->SetAnodePosition(  -lxemex/2.+halfinch/2.+lfro+lfr1+lfr2+lfr3);
      //In old version, needed anode + 0.01*mm hack.  
      field3->SetDriftVelocity(5*mm/microsecond); //returndriftv(Efield3z, density));
      field3->SetTransverseDiffusion(difftrans);
      field3->SetLongitudinalDiffusion(difflong);
      //field3->SetMaterial(HPXe);
      driftregion3 = new G4Region("driftregion3");
      driftregion3->SetUserInformation(field3);
      driftregion3->AddRootLogicalVolume(region3_log);
    }
  }

  
  
  void Next1Lbnl::DefineDaughterBoards()
  {
    const G4double sipm_pitch = 10. * mm; // Distance between adjacent sipms
    const G4double dbo_thickn =  5. * mm; // Thickness of the daughter-boards
    const G4double dbo_margin =  2. * mm; // Distance between adjacent boards
    
    const G4double coating_thickn = 1. * micrometer;

    G4Material* teflon = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

    G4Material* TPB = MaterialsList::TPB();
    TPB->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());

    // 4x4 DAUGHTER BOARD //////////////////////////////////////////////////////

    G4double width  = 4 * sipm_pitch - dbo_margin;
    G4double height = width;
    G4double thickn = dbo_thickn + coating_thickn;

    // TEFLON BOARD
    G4Box* dbo44_solid = new G4Box("DBO44", width/2., height/2., thickn/2.);
    _dbo44_logic = new G4LogicalVolume(dbo44_solid, teflon, "DBO44");

    // TPB COATING
    G4Box* coating44_solid =
      new G4Box("DBO_COATING", width/2., height/2., coating_thickn/2.);
    G4LogicalVolume* coating44_logic = 
      new G4LogicalVolume(coating44_solid, TPB, "DBO_COATING");
    
    G4double pos_z = (thickn - coating_thickn)/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., (thickn-coating_thickn)/2.),
		      coating44_logic, "DBO_COATING", _dbo44_logic, false, 0);

    // SILICON PMs
    SiPM11 sipm_geom; 
    G4LogicalVolume* sipm_logic = sipm_geom.GetLogicalVolume();

    pos_z = thickn/2. - coating_thickn - (sipm_geom.GetDimensions().z())/2.;
    G4double offset = sipm_pitch/2. - dbo_margin/2.;

    G4int sipm_no = 0.;
    
    for (G4int i=0; i<4; i++) {
      
      G4double pos_y = height/2. - offset - i*sipm_pitch;
      
      for (G4int j=0; j<4; j++) {
	
          G4double pos_x = -width/2. + offset + j*sipm_pitch;
          new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z), sipm_logic,
			  "SIPM11", _dbo44_logic, false, sipm_no);
          sipm_no++;
      }
    }

    // CORNER DAUGHTER BOARDS //////////////////////////////////////////////////

    G4double width42  = 4 * sipm_pitch - dbo_margin;
    G4double height42 = 2 * sipm_pitch - dbo_margin/2.;
    
    G4double width21  = 2 * sipm_pitch - dbo_margin/2.;
    G4double height21 = 1 * sipm_pitch - dbo_margin/2.;

    // TEFLON BOARD
    G4Box* dbo42_solid = 
      new G4Box("DBO_CORNER", width42/2., height42/2., thickn/2.);
    G4Box* dbo21_solid = 
      new G4Box("DBO_CORNER", width21/2., height21/2., thickn/2.);
    G4UnionSolid* dbocorner_solid = 
      new G4UnionSolid("DBO_CORNER", dbo42_solid, dbo21_solid, 0,
		       G4ThreeVector(width42/4., (height42/2.+height21/2.), 0));
    
    _dboCU_logic = new G4LogicalVolume(dbocorner_solid, teflon, "DBOCU");
    _dboCD_logic = new G4LogicalVolume(dbocorner_solid, teflon, "DBOCD");
    //CU = corner up, CD = corner down
    // TPB COATING 
    G4Box* coating42_solid =
      new G4Box("DBO_COATING", width42/2., height42/2., coating_thickn/2.);
    G4Box* coating21_solid =
      new G4Box("DBO_COATING", width21/2., height21/2., coating_thickn/2.);
    G4UnionSolid* coating_corner_solid =
      new G4UnionSolid("DBO_COATING", coating42_solid, coating21_solid, 0,
		       G4ThreeVector(width42/4., (height42/2.+height21/2.), 0.));
    
    G4LogicalVolume* coating_corner_logic =
      new G4LogicalVolume(coating_corner_solid, TPB, "DBO_COATING");

    pos_z = thickn/2. - coating_thickn/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), coating_corner_logic,
		      "DBO_COATING", _dboCU_logic, false, 0);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,-pos_z), coating_corner_logic,
		      "DBO_COATING", _dboCD_logic, false, 0);
    
    // SILICON PMs

    pos_z = thickn/2. - coating_thickn - (sipm_geom.GetDimensions().z())/2.;
    offset = sipm_pitch/2. - dbo_margin/2.;

    sipm_no = 0.;
    
    for (G4int i=0; i<3; i++) {
      
      G4double pos_y = height/2. - offset - i*sipm_pitch;
      
      for (G4int j=0; j<4; j++) {

	if (i==0) {
	  if (j==0 || j==1) 
	    continue;
	}
	
	G4double pos_x = -width/2. + offset + j*sipm_pitch;
	
	new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z), sipm_logic,
			  "SIPM11", _dboCU_logic, false, sipm_no);
	new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, -pos_z), sipm_logic,
			  "SIPM11", _dboCD_logic, false, sipm_no);

	sipm_no++;
      }
    }
  }
  
  // Generate a vertex at which particles are introduced to the chamber
  G4ThreeVector Next1Lbnl::GenerateVertex(const G4String& region) const
  {

    // Generate a vertex in the active region
    if (region == "ACTIVE") {
      return G4ThreeVector(0.,0.,5.181*cm);
    }
    // Generate vertices according to the current index in the EL table pattern constructed previously
    else if (region == "EL_TABLE") {


      // Must pass through the table at least one time
      if(_numb_of_events<_table_vertices.size()){
        G4cout<<"number events too small, you need at least "<<_table_vertices.size()
	        <<" events to generate EL table"<<G4endl;
        G4cout<<"[Next1EL] Aborting the run ..."<<G4endl;
        G4RunManager::GetRunManager()->AbortRun();
      } 
      // Pull the next event from the table
      else {
        _idx_table++;	
        if(_idx_table>=_table_vertices.size()){
	      G4cout<<"[Next1EL] Aborting the run, last event reached ..."<<G4endl;
	      G4RunManager::GetRunManager()->AbortRun();
        }
        if(_idx_table<=_table_vertices.size()){
          G4cout<<"Table construction vertex: "<<_idx_table<<"\n";
	      //return G4ThreeVector(0,0,65);
          return _table_vertices[_idx_table-1];
        }
      }
    }
    
  }

  // Create the table of initial points at which to drop ionielectrons for the EL table
  void Next1Lbnl::CalculateELTableVertices(G4double xlen, G4double ylen, G4double binning, G4double z)					
  {     
    G4ThreeVector position;

    // Set the z-position of the plane
    position[2] = z;

    // Calculate the maximum x and y indices
    G4int ixmax = floor(2*xlen/binning);
    G4int iymax = floor(2*ylen/binning);
    G4cout << "ixmax = " << ixmax << ", iymax = " << iymax << G4endl;

    // Open a debug file
    ofstream f_debug("debug/debug_ELpoints.dat",ios::out);

    // Construct the table of positions
    for (int i=0; i<ixmax; i++){
      position[0] = -xlen + i*binning;
      for (int j=0; j<iymax; j++){
	    position[1] = -ylen + j*binning;
        f_debug << "[Pt " << (iymax*i + j) << "] = (" << position[0]
                << ", " << position[1] << ", " << position[2] << ")\n";
        _table_vertices.push_back(position);
      }
    }

    f_debug.close();
  }
 
  //Little function to approximate drift velocity in xenon gas:
  G4double Next1Lbnl::returndriftv(G4double Efield, G4double density) 
  {
    G4double mureduce = 2.8E24 / (volt * m * second);
    G4double Xemass = 131.293*1.660E-27*kg;
    return fabs(mureduce*Efield/(density/(Xemass)));
  }
  
} // end namespace nexus
