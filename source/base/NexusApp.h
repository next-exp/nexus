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
namespace nexus { class GeometryFactory; }
namespace nexus { class GeneratorFactory; }


namespace nexus {

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


  private:
    G4GenericMessenger* _msg;

    GeometryFactory* _geomfctr;
    GeneratorFactory* _genfctr;

    std::vector<G4String> _macros;
  };

} // namespace nexus

#endif