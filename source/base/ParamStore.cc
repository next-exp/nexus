// ----------------------------------------------------------------------------
//  $Id$
//  
//  Author : J Martin-Albo <jmalbos@ific.uv.es>
//  Created: 29 May 2009
//
//  Copyright (c) 2009-2012 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ParamStore.h"

#include <bhep/sreader.h>
#include <bhep/bhep_svc.h>


namespace nexus {

  
  ParamStore::ParamStore(const G4String& keyword):
    _store(new bhep::gstore()), _keyword(keyword)
  {
  }
  
  
  
  ParamStore::~ParamStore()
  {
    delete _store;
  }
  
  
  
  void ParamStore::ParseFile(const G4String& filename)
  {
    bhep::sreader parser(*_store);
    parser.file(filename);
    parser.info_level(bhep::MUTE);
    parser.group(_keyword);
    parser.read();
  }
  
  
  
  void ParamStore::ExpandConfigFile()
  {
    if (_store->find_sstore("config_file")) {
      G4String filename = _store->fetch_sstore("config_file");
      ParseFile(filename);
      _store->erase_sstore("config_file");
    }
  }
  
  
  
  G4int ParamStore::GetIParam(const G4String& name) const
  {
    try {
      return _store->fetch_istore(name);
    }
    catch (bhep::internal_logic& e) {
      Error(name);
    }
  }
  
  G4double ParamStore::GetDParam(const G4String& name) const
  {
    try {
      return _store->fetch_dstore(name);
    }
    catch (bhep::internal_logic& e) {
      Error(name);
    }
  }
    
  G4String ParamStore::GetSParam(const G4String& name) const
  {
    try {
      return _store->fetch_sstore(name);
    }
    catch (bhep::internal_logic& e) {
      Error(name);
    }
  }
    
  const bhep::vdouble& ParamStore::GetDVParam(const G4String& name) const
  {
    try {
      return _store->fetch_vstore(name);
    }     
    catch (bhep::internal_logic& e) {
      Error(name);
    }
  }

  const bhep::vstring& ParamStore::GetSVParam(const G4String& name) const
  {
    try {
      return _store->fetch_svstore(name);
    }
    catch (bhep::internal_logic& e) {
      Error(name);
    }
  }


  G4bool ParamStore::PeekIParam(const G4String& name) const
  {
    return _store->find_istore(name);
  }

  G4bool ParamStore::PeekDParam(const G4String& name) const
  {
    return _store->find_dstore(name);
  }

  G4bool ParamStore::PeekSParam(const G4String& name) const
  {
    return _store->find_sstore(name);
  }
  
  G4bool ParamStore::PeekDVParam(const G4String& name) const
  {
    return _store->find_vstore(name);
  }
  
  G4bool ParamStore::PeekSVParam(const G4String& name) const
  {
    return _store->find_svstore(name);
  }
  
  
  
  void ParamStore::Error(const G4String& name) const
  {
    G4String msg = " ERROR: Requested param '" + name 
      + "' could not be found in the store " + _keyword + ".";
    G4Exception("[ParamStore]", "Error()", FatalException, msg);
  }
  
  
  
  void ParamStore::Save() const
  {
    // trick to get a readable property name
    G4String tag = _keyword + "_";
    
    // save all params
    bhep::bhep_svc::instance()->
      get_run_info().add_properties(*_store, tag);
  }
  

} // end namespace nexus
