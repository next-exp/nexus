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

#ifndef __NEXUS_APP__
#define __NEXUS_APP__

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


  private:
    G4GenericMessenger* _msg;

    GeometryFactory*  _geom_fctr;
    GeneratorFactory* _gen_fctr;
    ActionsFactory*   _act_fctr;

    std::vector<G4String> _macros;

    G4int _random_seed;
  };

} // namespace nexus

#endif
