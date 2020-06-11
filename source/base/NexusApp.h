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

    /// Returns the number of events to be processed in the current run
    G4int GetNumberOfEventsToBeProcessed() const;

  private:
    void RegisterMacro(G4String);

    void RegisterDelayedMacro(G4String);

    void ExecuteMacroFile(const char*);

    /// Set a seed for the G4 random number generator. 
    /// If a negative value is chosen, the system time is set as seed.
    void SetRandomSeed(G4int);

  private:
    G4GenericMessenger* msg_;
    std::vector<G4String> macros_;
    std::vector<G4String> delayed_;

    GeneratorFactory* genfctr_;

  };

  // INLINE DEFINITIONS ////////////////////////////////////

  inline G4int NexusApp::GetNumberOfEventsToBeProcessed() const
  { return numberOfEventToBeProcessed; }

} // namespace nexus

#endif
