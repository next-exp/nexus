// NEXUS: manager‰.cc
//    -- Created on: June 5 2007
//    -- Last modification: June 2, 2008 (jmalbos)
//

#include <nexus/manager.h>
#include <bhep/sreader.h>



namespace nexus {

  /// constructor
  manager::manager( std::string params_filename, bhep::prlevel vl )
  {
    __params_filename = params_filename;
    __init            = false;
   
    __msg = bhep::messenger(vl);
    __msg.message("[nexus::manager] Class instance created.", bhep::DUMP);
  }

  
  /// destructor
  manager::~manager()
  {
    delete __evt_mgr;
    __msg.message("[nexus::manager] Class instance deleted.", bhep::DUMP);
  }


  /// init method
  void manager::initialize()
  {
    set_defaults();
    set_params_stores();
    set_event_mgr();
    set_odst_header();
    
    __init = true; // manager instance initialized
  }


  /// set default values for some parameters
  void manager::set_defaults()
  {
    __idst_filename    = "";
    __odst_filename    = "../data/nexus_output.gz";
    __verbosity        = "NORMAL";
    __G4_verbosity     = "0";
    __energy_loss      = 1;
    __mult_scattering  = 1;
    __evt_mgr          = 0;
  }


  /// sets stores with input parameters
  void manager::set_params_stores()
  {
    // global parameters
    read_params(__global_store, __params_filename, "GLOBAL");
    check_global_params();
    
    // geometry parameters
    read_params(__geom_store, __global_store.fetch_sstore("geom_filename"), "GEOMETRY");
    check_geom_params();

    // physics parameters
    read_params(__physics_store, __params_filename, "PHYSICS");
    check_physics_params();

    // generator parameters
    read_params(__generator_store, __params_filename, "GENERATOR");
    check_generator_params();
  }


  /// reads parameters from file
  void manager::read_params(bhep::gstore& store, std::string filename, std::string group)
  {
    bhep::sreader reader(store);
    reader.file(filename);
    reader.info_level(bhep::NORMAL);
    reader.group(group);
    reader.read();
  }


  /// set bhep EventManager
  void manager::set_event_mgr()
  {
    bhep::gstore store;
    bhep::vstring idst_files, odst_files;
    
    odst_files.push_back( __global_store.fetch_sstore("odst_filename") );
    if( __idst_filename!="" )
      idst_files.push_back( __idst_filename );
    
    store.store("idst_files", idst_files);
    store.store("odst_files", odst_files);
    
    __evt_mgr = new EventManager2(store, bhep::MUTE);
    __evt_mgr->initialize();
  }

  
  void manager::check_global_params()
  {
    // defaults
    if( !__global_store.find_sstore("verbosity") )
      __global_store.store( "verbosity", __verbosity );
    if( !__global_store.find_sstore("tracking_verbosity") )
      __global_store.store( "tracking_verbosity", __G4_verbosity );
    if( !__global_store.find_sstore("odst_filename") )
      __global_store.store( "odst_filename", __odst_filename );

    // mandatory
    assert_option( __global_store.find_istore("num_events"), "GLOBAL num_events" );
    assert_option( __global_store.find_sstore("geom_filename"), "GLOBAL geom_filename" );
    assert_option( __global_store.find_istore("trigger"), "GLOBAL trigger" );
  }


  void manager::check_physics_params()
  {
    // defaults
    if( !__physics_store.find_istore("energy_loss") ) 
      __physics_store.store("energy_loss", __energy_loss );
    if( !__physics_store.find_istore("mult_scattering") )
      __physics_store.store("mult_scattering", __mult_scattering );
      
    // mandatory
    assert_option( __physics_store.find_dstore("bulk_max_step"), "PHYSICS bulk_max_step" );
    assert_option( __physics_store.find_dstore("bulk_cut"), "PHYSICS bulk_cut" );
  }


  void manager::check_geom_params()
  {
    std::string chamber_shape;
    try {
      chamber_shape = __geom_store.fetch_sstore("CHAMBER_shape"); }
    catch( bhep::internal_logic ) {
      assert_option( false, "GEOMETRY CHAMBER_shape" ); }

    if( chamber_shape!="box" && chamber_shape!="cylinder" )
      option_is_not_valid( "CHAMBER_shape", chamber_shape );

    assert_option( __geom_store.find_vstore("CHAMBER_dimensions"), "GEOMETRY CHAMBER_dimensions" );
    assert_option( __geom_store.find_dstore("IVESSEL_thickness"), "GEOMETRY IVESSEL_thickness" );
    assert_option( __geom_store.find_dstore("OVESSEL_thickness"), "GEOMETRY OVESSEL_thickness" );
    assert_option( __geom_store.find_dstore("BUFFER_gap"), "GEOMETRY BUFFER_gap" );
    assert_option( __geom_store.find_dstore("pressure"), "GEOMETRY pressure" );

    bhep::vdouble foo = __geom_store.fetch_vstore("CHAMBER_dimensions") * cm;
    __geom_store.change_vstore( "CHAMBER_dimensions", foo );

    double foo2 = __geom_store.fetch_dstore("pressure") * atmosphere;
    __geom_store.change_dstore( "pressure", foo2 );

    foo2 = __geom_store.fetch_dstore("OVESSEL_thickness") * cm;
    __geom_store.change_dstore( "OVESSEL_thickness", foo2 );

    foo2 = __geom_store.fetch_dstore("IVESSEL_thickness") * cm;
    __geom_store.change_dstore( "IVESSEL_thickness", foo2 );

    foo2 = __geom_store.fetch_dstore("BUFFER_gap") * cm;
    __geom_store.change_dstore( "BUFFER_gap", foo2 );
  }

  
  void manager::check_generator_params()
  {
    std::string gen_type;
    try { 
      gen_type = __generator_store.fetch_sstore("type"); } 
    catch( bhep::internal_logic ) {
      assert_option( false, "GENERATOR type" ); }
    
    if( gen_type == "single_particle" ) {
      assert_option( __generator_store.find_dstore("min_energy"), "GENERATOR min_energy" );
      assert_option( __generator_store.find_dstore("max_energy"), "GENERATOR min_energy" );
      assert_option( __generator_store.find_sstore("particle_name"), "GENERATOR particle_name" );
    }
    else if( gen_type == "idst" ) {
      assert_option( __generator_store.find_sstore("idst_filename"), "GENERATOR idst_filename" );
      __idst_filename = __generator_store.fetch_sstore("idst_filename"); // storing filename to set EventManager
    }
    else
      option_is_not_valid( "GENERATOR type", gen_type );

    
    std::string vtx_type;
    try { 
      vtx_type = __generator_store.fetch_sstore("vtx_type"); } 
    catch( bhep::internal_logic ) {
      assert_option( false, "GENERATOR vtx_type" ); }
    
    if( vtx_type == "spot" )
      assert_option( __generator_store.find_vstore("spot"), "GENERATOR spot" );
    
    else if( vtx_type == "random" ) {
      std::string region;
      try { 
	region = __generator_store.fetch_sstore("region"); 
	if( region!="active" && region!="ivessel" && region!="ovessel" )
	  option_is_not_valid( "region", region ); } 
      catch( bhep::internal_logic ) {
	assert_option( false, "GENERATOR region" ); }
    }
    else
      option_is_not_valid( "GENERATOR vtx_type", vtx_type );
  }


  void manager::set_odst_header()
  {
    __evt_mgr->add_run_properties( __geom_store, "GEOMETRY_" );
  }

  
  void manager::assert_option( bool ok, const std::string& option )
  {
    if( !ok ) {
      std::ostringstream err_msg;
      err_msg << "\nERROR.- manager: Option not found in parameters file: "
	      << option << std::endl;
      throw std::runtime_error(err_msg.str());
    }
  }

  
  void manager::option_is_not_valid( const std::string& option, const std::string& value )
  {
    std::ostringstream err_msg;
    err_msg << "\nERROR.- manager: Invalid option: " << option << " = "
	    << value << std::endl;
    throw std::runtime_error(err_msg.str());
  }
  

  void manager::assert_init( const std::string& where )
  {
    if(!__init) {
      std::ostringstream err_msg;
      err_msg << "\nERROR.- " << where << ": Manager not initialized!"
		<< std::endl;
      throw std::runtime_error(err_msg.str());
    }
  }


  /// Some useful getters...
  bhep::prlevel manager::get_verbosity()
  {
    assert_init( "manager::get_verbosity()" );
    return bhep::get_info_level( __global_store.fetch_sstore("verbosity") );
  }

  std::string manager::get_tracking_verbosity()
  {
    assert_init( "manager::get_tracking_verbosity()" );
    return __global_store.fetch_sstore("tracking_verbosity");
  }

  int manager::get_num_events()
  {
    assert_init( "manager::get_num_events()" );
    return __global_store.fetch_istore("num_events");
  }

  int manager::get_trigger()
  {
    assert_init( "manager::get_trigger()" );
    return __global_store.fetch_istore("trigger");
  }


  const bhep::gstore& manager::get_global_store()
  {
    assert_init( "manager::get_global_store()" );
    return __global_store;
  }


  const bhep::gstore& manager::get_geom_store()
  {
    assert_init( "manager::get_geom_store()" );
    return __geom_store;
  }

  
  const bhep::gstore& manager::get_physics_store()
  {
    assert_init( "manager::get_physics_store()" );
    return __physics_store;
  }

  
  const bhep::gstore& manager::get_generator_store()
  {
    assert_init( "manager::get_generator_store()" );
    return __generator_store;
  }
  
  
  const std::string& manager::get_geom_filename()
  {
    assert_init( "manager::get_geom_filename()" );
    return __geom_filename;
  }

  EventManager2& manager::get_event_mgr()
  {
    assert_init( "manager::get_event_mgr()" );
    return *__evt_mgr;
  }

} // namespace nexus
