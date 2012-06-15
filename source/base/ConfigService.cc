// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 25 Mar 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "ConfigService.h"


namespace nexus {
  
  
  ConfigService::ConfigService():
    _job       (new ParamStore("JOB")), 
    _geometry  (new ParamStore("GEOMETRY")),
    _generation(new ParamStore("GENERATION")), 
    _physics   (new ParamStore("PHYSICS")),
    _actions   (new ParamStore("ACTIONS"))
  {
  }
  
  
  
  ConfigService::~ConfigService()
  {
    delete _job;
    delete _geometry;
    delete _generation;
    delete _physics;
    delete _actions;
  }
  
  
  
  ConfigService& ConfigService::Instance()
  {
    // The singleton instance is a static object (Meyer's singleton). 
    // It is initialized the first time Instance() is called and 
    // deallocated when the program finishes. 
    static ConfigService svc;
    return svc;
  }
  
  
  
  void ConfigService::SetConfigFile(const G4String& config_file)
  {
    // Read general config file and fill corresponding stores
    _job       ->ParseFile(config_file);
    _geometry  ->ParseFile(config_file);
    _generation->ParseFile(config_file);
    _physics   ->ParseFile(config_file);
    _actions   ->ParseFile(config_file);
    
    // Expand additional config files if needed
    _geometry  ->ExpandConfigFile();
    _generation->ExpandConfigFile();
    _physics   ->ExpandConfigFile();
    _actions   ->ExpandConfigFile();
  }
  

} // namespace nexus
