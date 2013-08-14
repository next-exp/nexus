// NEXUS: geometry.cc
//    -- Created on Feb 2007
//    -- Last modification: April 21 2008, jmalbos
//

#include <nexus/geometry.h>

#include "G4Box.hh"
#include "G4Tubs.hh"

#include "G4UserLimits.hh"
#include "G4PVPlacement.hh"


namespace nexus {


  /// Constructor
  geometry::geometry( const bhep::gstore& geom_store, 
		      const bhep::gstore& physics_store, bhep::prlevel vl )
  { 
    __geom_store    = new bhep::gstore(geom_store);
    __physics_store = new bhep::gstore(physics_store);
    
    msg = bhep::messenger(vl);
    
    read_geom_params();
    read_physics_params();
    calculate_dimensions();

    msg.message("nexus::geometry instance created.", bhep::VERBOSE);
  }

  
  /// Destructor
  geometry::~geometry()
  {
    delete __geom_store;
    delete __physics_store;
    msg.message("nexus::geometry instance deleted.", bhep::VERBOSE);
  }


  void geometry::read_geom_params()
  {
    //__CHAMBER_shape      = __geom_store->fetch_sstore("CHAMBER_shape");
    __ACTIVE_dimensions  = __geom_store->fetch_vstore("CHAMBER_dimensions");
    __IVESSEL_thickness  = __geom_store->fetch_dstore("IVESSEL_thickness");
    __OVESSEL_thickness  = __geom_store->fetch_dstore("OVESSEL_thickness");
    __BUFFER_gap         = __geom_store->fetch_dstore("BUFFER_gap");
    __pressure           = __geom_store->fetch_dstore("pressure");
  }


  void geometry::read_physics_params()
  {
    __bulk_max_step = __physics_store->fetch_dstore("bulk_max_step") * mm;
  }


  void geometry::calculate_dimensions()
  {
    for( size_t i=0; i<3; i++) { 
      // Inner vessel 
      __IVESSEL_dimensions.push_back( __ACTIVE_dimensions[i]  + __IVESSEL_thickness*2. );
      // Buffer gas
      __BUFFER_dimensions.push_back( __IVESSEL_dimensions[i] + __BUFFER_gap*2. );
      // Outer vessel
      __OVESSEL_dimensions.push_back( __BUFFER_dimensions[i]  + __OVESSEL_thickness*2. );
    }
  }


  /// Geant4 mandatory method
  G4VPhysicalVolume* geometry::Construct()
  {
    msg.message("\n", bhep::NORMAL);
    msg.message("-------------------------------------------------", bhep::NORMAL);
    msg.message("  GEOMETRY CONSTRUCTION                          ", bhep::NORMAL);
    msg.message("-------------------------------------------------", bhep::NORMAL);
  
    msg.message("  Defining materials.", bhep::DETAILED);
    define_materials();

    msg.message("  Defining volumes.", bhep::DETAILED);
    define_world();
    define_ovessel();
    define_buffer();
    define_ivessel();
    define_active();
    
    msg.message("  Defining regions.", bhep::DETAILED);
    define_regions();

    msg.message("  Setting visibilities.", bhep::DETAILED);
    set_visibilities();

    msg.message("-------------------------------------------------\n", bhep::DETAILED);
   
    return physi_WORLD;
  }

  
  void geometry::define_world()
  {
    G4double X, Y, Z;
    X = __OVESSEL_dimensions[0] + 3.*m;
    Y = __OVESSEL_dimensions[1] + 3.*m;
    Z = __OVESSEL_dimensions[2] + 3.*m;
    
    solid_WORLD = new G4Box("WORLD", X/2., Y/2., Z/2.);

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
  }
  
  
  void geometry::define_ovessel()
  {
    solid_OVESSEL = new G4Box("OVESSEL", __OVESSEL_dimensions[0]/2., 
			      __OVESSEL_dimensions[1]/2., __OVESSEL_dimensions[2]/2.);
    logic_OVESSEL = new G4LogicalVolume(solid_OVESSEL, Inox, "OVESSEL", 0, 0, 0, true); 
    physi_OVESSEL = new G4PVPlacement(0, G4ThreeVector(0,0,0), logic_OVESSEL,
				      "OVESSEL", logic_WORLD, false, 0);
  }  
  

  void geometry::define_buffer( )
  {
    solid_BUFFER = new G4Box("BUFFER", __BUFFER_dimensions[0]/2., __BUFFER_dimensions[1]/2., 
			     __BUFFER_dimensions[2]/2.);
    logic_BUFFER = new G4LogicalVolume(solid_BUFFER, Air, "BUFFER", 0, 0, 0, true); 
    physi_BUFFER = new G4PVPlacement(0, G4ThreeVector(0,0,0), logic_BUFFER,
				      "BUFFER", logic_OVESSEL, false, 0);
  }


  void geometry::define_ivessel()
  {
    solid_IVESSEL = new G4Box("IVESSEL", __IVESSEL_dimensions[0]/2., __IVESSEL_dimensions[1]/2., 
			      __IVESSEL_dimensions[2]/2.);
    logic_IVESSEL = new G4LogicalVolume(solid_IVESSEL, Cu, "IVESSEL", 0, 0, 0, true); 
    physi_IVESSEL = new G4PVPlacement(0, G4ThreeVector(0,0,0), logic_IVESSEL,
				      "IVESSEL", logic_BUFFER, false, 0);
  }


  void geometry::define_active()
  {
    solid_ACTIVE = new G4Box("ACTIVE", __ACTIVE_dimensions[0]/2., __ACTIVE_dimensions[1]/2., 
			     __ACTIVE_dimensions[2]/2.);
    logic_ACTIVE = new G4LogicalVolume(solid_ACTIVE, HPXe, "ACTIVE", 0, 0, 0, true); 
    
    // Setting Maximum Step Length
    logic_ACTIVE->SetUserLimits(new G4UserLimits(__bulk_max_step));
    
    physi_ACTIVE = new G4PVPlacement(0, G4ThreeVector(0,0,0), logic_ACTIVE,
				     "ACTIVE", logic_IVESSEL, false, 0);
  }
  
  
  void geometry::define_regions()
  {
    bulk_region = new G4Region("BulkRegion");
    bulk_region->AddRootLogicalVolume(logic_ACTIVE);
  }
   

  bhep::vdouble geometry::get_active_dimensions()
  {
    return __ACTIVE_dimensions;
  }

  bhep::vdouble geometry::get_ivessel_dimensions()
  {
    return __IVESSEL_dimensions;
  }

  bhep::vdouble geometry::get_ovessel_dimensions()
  {
    return __OVESSEL_dimensions;
  }

  bhep::vdouble geometry::get_buffer_dimensions()
  {
    return __BUFFER_dimensions;
  }
  
} // namespace nexus
