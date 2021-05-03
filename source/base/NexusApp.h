// ----------------------------------------------------------------------------
// nexus | NexusApp.h
//
// This class is the run manager of the nexus simulation. It takes care of
// setting up the simulation (geometry, physics lists, generators, actions),
// so that it is ready to be run.
//
// The NEXT Collaboration
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
    G4String gen_name_; ///< Name of the chosen primary generator
    G4String pm_name_;  ///< Name of the chosen persistency manager
    G4String geo_name_;  ///< Name of the chosen geometry

    std::vector<G4String> macros_;
    std::vector<G4String> delayed_;

  };

  // INLINE DEFINITIONS ////////////////////////////////////

  inline G4int NexusApp::GetNumberOfEventsToBeProcessed() const
  { return numberOfEventToBeProcessed; }

} // namespace nexus

#endif
