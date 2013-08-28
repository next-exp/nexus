// ----------------------------------------------------------------------------
///  \file   NexusApp.h
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     8 March 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef NEXUS_APP_H
#define NEXUS_APP_H

#include <G4RunManager.hh>

class G4GenericMessenger;


namespace nexus {

  class GeometryFactory;
  class GeneratorFactory;
  class ActionsFactory;


  /// TODO. CLASS DESCRIPTION

  class NexusApp: public G4RunManager
  {
  public:
    /// Constructor
    NexusApp(G4String init_macro);
    /// Destructor
    ~NexusApp();

    virtual void Initialize();

  private:
    void RegisterMacro(const G4String&);

    void ExecuteMacroFile(const char*);

    /// Set a seed for the G4 random number generator. 
    /// If a negative value is chosen, the system time is set as seed.
    void SetRandomSeed(G4int);

  private:
    G4GenericMessenger* _msg;
    std::vector<G4String> _macros;
  };

} // namespace nexus

#endif
