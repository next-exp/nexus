// NEXT simulation: geometry.cc
// Implementation of class geometry, Feb 2006
//


#include <next/geometry.hh>



//using namespace bhep;


/// === Constructor ======
geometry::geometry(G4String geom_file)
{ 
  geom_file_ = geom_file;
}


/// === Implementing the geometry ======
G4VPhysicalVolume* geometry::Construct()
{

  m_.message("\n***************************************", bhep::CONCISE);
  m_.message(  "******** GEOMETRY CONSTRUCTION ********", bhep::CONCISE);
  m_.message(  "***************************************", bhep::CONCISE);
  m_.message("\n** Reading parameters from", geom_file_, ":", bhep::CONCISE);
  
  
  // ---- Reading geometry parameters file ------------
  
  // create a generic store
  bhep::gstore store;
  
  // and one reader per group
  bhep::sreader reader(store);
  reader.file(geom_file_);
  reader.info_level(bhep::NORMAL);
  reader.group("GEOMETRY");
  reader.read();

  // Time Projection CHAMBER
  G4double CHAMBER_length = store.fetch_dstore("CHAMBER_length") * cm;
  m_.message("   Tracking CHAMBER length set to:", CHAMBER_length/cm, "cm", bhep::CONCISE);
  
  G4double CHAMBER_height = store.fetch_dstore("CHAMBER_height") * cm;
  m_.message("   Tracking CHAMBER height set to:", CHAMBER_height/cm, "cm", bhep::CONCISE);
  
  G4double CHAMBER_width = store.fetch_dstore("CHAMBER_width") * cm;
  m_.message("   Tracking CHAMBER width set to:", CHAMBER_width/cm, "cm", bhep::CONCISE);

  // VETOed TPC gap
  G4double VETO = store.fetch_dstore("VETO") * cm;
  m_.message("   VETOed TPC gap set to:", VETO/cm, "cm", bhep::CONCISE);


  // ---- Reading physics parameters file ------------

  // another reader
  bhep::sreader reader2(store);
  reader2.file("next.par");
  reader2.group("PHYSICS");
  reader2.read();
  
  // Maximum Step Length in the SOURCE
  G4double max_step = store.fetch_dstore("max_step") * mm;
  m_.message("   Maximum Step Length in the CHAMBER set to:", max_step/mm, "mm", bhep::CONCISE);

  
  // ---- Materials definition ------------
  
  define_materials();
  
  
  // ---- Dimensions setting ------------
  
  m_.message("\n** Setting dimensions ...", bhep::VERBOSE);

  // Time Projection CHAMBER
  G4double CHAMBER_X = CHAMBER_width;
  G4double CHAMBER_Y = CHAMBER_height;
  G4double CHAMBER_Z = CHAMBER_length;
  
  // ACTIVE TPC volume (non-VETOed)
  G4double ACTIVE_X = CHAMBER_X - VETO;
  G4double ACTIVE_Y = CHAMBER_Y - VETO;
  G4double ACTIVE_Z = CHAMBER_Z - VETO;
  
  // The entire DETECTOR
  G4double DETECTOR_X = CHAMBER_X + 50. * cm;
  G4double DETECTOR_Y = CHAMBER_Y + 50. * cm;
  G4double DETECTOR_Z = CHAMBER_Z + 50. * cm;
  
  // MOTHER VOLUME (20cm bigger than the detector)
  G4double WORLD_X = DETECTOR_X + 20. * cm;
  G4double WORLD_Y = DETECTOR_Y + 20. * cm;
  G4double WORLD_Z = DETECTOR_Z + 20. * cm;
  
  // VERBOSING
  m_.message("   WORLD: X Y Z dimensions (mm): ", WORLD_X/mm, WORLD_Y/mm, WORLD_Z/mm, bhep::VERBOSE);
  m_.message("   DETECTOR: X Y Z dimensions (mm): ", DETECTOR_X/mm, DETECTOR_Y/mm, DETECTOR_Z/mm, bhep::VERBOSE);
  m_.message("   Time Projection CHAMBER: X Y Z dimensions (mm): ", CHAMBER_X/mm, CHAMBER_Y/mm, CHAMBER_Z/mm, bhep::VERBOSE);
  
  
  
  // ---- Definitions of Solids, Logical Volumes, Physical Volumes ------------
  
  m_.message("\n** Defining volumes ...", bhep::NORMAL);
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Mother Volume.  Name: WORLD  Shape: Box
  // Position: Center of Our Universe
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  m_.message("   WORLD", bhep::DETAILED);
    
  solid_WORLD = new G4Box("WORLD", WORLD_X/2., WORLD_Y/2., WORLD_Z/2.);
    
  logic_WORLD = new G4LogicalVolume(solid_WORLD,   // Solid
				    Vacuum,        // Material
				    "WORLD",       // Name
				    0,             // Field Manager
				    0,             // Sensitive
				    0,             // User limits
				    true);         // Optimisation
  
  physi_WORLD = new G4PVPlacement(0,                // no rotation
				  G4ThreeVector(),  // Translation with respect to its mother volume
				  logic_WORLD,      // its logical volume
				  "WORLD",          // its name
				  0,                // its mother logical volume
				  false,            // no boolean operations
				  0);               // copy number

    
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Detector.  Name: DETECTOR  Shape: Box
  // Position: Inside WORLD
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
  m_.message("   DETECTOR", bhep::DETAILED);
    
  solid_DETECTOR = new G4Box("DETECTOR", DETECTOR_X/2., DETECTOR_Y/2., DETECTOR_Z/2.);

  logic_DETECTOR = new G4LogicalVolume(solid_DETECTOR, Vacuum, "DETECTOR", 0, 0, 0, true); 
    
  physi_DETECTOR = new G4PVPlacement(0, G4ThreeVector(0,0,0), logic_DETECTOR,
				     "DETECTOR", logic_WORLD, false, 0);


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  // Xe Time Projection Chamber. Name: CHAMBER  Shape: Box
  // Position: Inside DETECTOR
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
  m_.message("   CHAMBER", bhep::DETAILED);
  
  solid_CHAMBER = new G4Box("CHAMBER", CHAMBER_X/2., CHAMBER_Y/2., CHAMBER_Z/2.);
    
  logic_CHAMBER = new G4LogicalVolume(solid_CHAMBER, TPC_Xenon, "CHAMBER", 0, 0, 0, true);

  // Setting Maximum Step Length
  logic_CHAMBER->SetUserLimits(new G4UserLimits(max_step));
    
  physi_CHAMBER = new G4PVPlacement(0, G4ThreeVector(0,0,0), logic_CHAMBER,
				    "CHAMBER", logic_DETECTOR, false, 0);
  

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  // Allowed tracking volume.  Name: ACTIVE  Shape: Box
  // Position: Inside CHAMBER
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  m_.message("   ACTIVE tracking volume", bhep::DETAILED);
  
  solid_ACTIVE = new G4Box("ACTIVE", ACTIVE_X/2., ACTIVE_Y/2., ACTIVE_Z/2.);
    
  logic_ACTIVE = new G4LogicalVolume(solid_ACTIVE, TPC_Xenon, "ACTIVE", 0, 0, 0, true);
    
  physi_ACTIVE = new G4PVPlacement(0, G4ThreeVector(0,0,0), logic_ACTIVE,
				     "ACTIVE", logic_CHAMBER, false, 0);

  // Setting Maximum Step Length
  logic_ACTIVE->SetUserLimits(new G4UserLimits(max_step));
  
  // ---- Regions definition ------------
      
  bulk_region = new G4Region("Bulk_region");
  bulk_region->AddRootLogicalVolume(logic_CHAMBER);

    
  // ---- Visualization attributes ------------
  
  m_.message("\n** Setting visibilities ...", bhep::NORMAL);
  set_visibilities();

  m_.message("***************************************\n", bhep::CONCISE);

  return physi_WORLD;
}

