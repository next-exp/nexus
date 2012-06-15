// ----------------------------------------------------------------------------
///  \file   ConfigService.h
///  \brief  Singleton for run-time configuration management.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     25 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __CONFIG_SERVICE__
#define __CONFIG_SERVICE__

#include "ParamStore.h"


namespace nexus {

  /// Singleton class for run-time configuration management. It provides
  /// global access to the user configuration parameters.
  
  class ConfigService
  {
  public:
    /// Returns the singleton instance
    static ConfigService& Instance();
    
    /// Set general config file
    void SetConfigFile(const G4String&);

    /// Get the geometry-parameters store
    const ParamStore& Geometry() const;
    /// Get the generation-parameters store
    const ParamStore& Generation() const;
    /// Get the physics-parameters store
    const ParamStore& Physics() const;
    /// Get the actions-parameters store
    const ParamStore& Actions() const;
    /// Get the job-parameters store
    const ParamStore& Job() const;
    
  private:
    // Ctors, dtor and assig-op are hidden
    ConfigService();
    ~ConfigService();
    ConfigService(const ConfigService&);
    ConfigService& operator =(const ConfigService&);

  private:
    ParamStore* _geometry;
    ParamStore* _generation;
    ParamStore* _physics;
    ParamStore* _actions;
    ParamStore* _job;
  };

  // inline methods ..................................................

  inline const ParamStore& ConfigService::Geometry() const 
  { return *_geometry; }
  
  inline const ParamStore& ConfigService::Generation() const 
  { return *_generation; }

  inline const ParamStore& ConfigService::Physics() const
  { return *_physics; }

  inline const ParamStore& ConfigService::Actions() const
  { return *_actions; }

  inline const ParamStore& ConfigService::Job() const
  { return *_job; }

} // namespace nexus

#endif
