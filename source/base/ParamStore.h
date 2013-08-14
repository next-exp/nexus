// ----------------------------------------------------------------------------
///  \file   ParamStore.h
///  \brief  A collection of configuration parameters. 
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     29 May 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __PARAM_STORE__
#define __PARAM_STORE__

#include <globals.hh>
#include <bhep/gstore.h>


namespace nexus {

  /// This class encapsulates a collection of configuration parameters. 

  class ParamStore
  {
  public:
    /// Constructor
    ParamStore(const G4String& keyword);
    /// Destructor
    ~ParamStore();

    /// Parse file and fill store
    void ParseFile(const G4String&);
    /// 
    void ExpandConfigFile();

    /// Return an integer from the parameter store
    G4int GetIParam(const G4String&) const;
    /// Return a double from the parameter store
    G4double GetDParam(const G4String&) const;
    /// Return a string from the parameter
    G4String GetSParam(const G4String&) const;
    /// Return a vdouble from the store
    const bhep::vdouble& GetDVParam(const G4String&) const;
    /// Returns a vector of strings from the store
    const bhep::vstring& GetSVParam(const G4String&) const;

    // Check whether param exists (true) or not (false) in store
    G4bool PeekIParam(const G4String&) const;
    G4bool PeekDParam(const G4String&) const;
    G4bool PeekSParam(const G4String&) const;
    G4bool PeekDVParam(const G4String&) const;
    G4bool PeekSVParam(const G4String&) const;

    void Save() const;


  private:
    void Error(const G4String&) const;

  private:
    G4String _keyword;
    bhep::gstore* _store;
  };
  
} // end namespace nexus

#endif
