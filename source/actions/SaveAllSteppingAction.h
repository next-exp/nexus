// ----------------------------------------------------------------------------
// nexus | SaveAllSteppingAction.h
//
// This class adds a new group and table to the output file, "/DEBUG/steps".
// This table contains information (position and volume of both the
// pre- and post-step points, average time, process name and other identifiers)
// of some steps of the simulation. By default all steps are stored. However,
// a subset of them can be selected by cherry-picking the volumes and particles
// involved in the step. This can be achieved with the commands
// /Actions/SaveAllSteppingAction/select_particle
// and
// /Actions/SaveAllSteppingAction/select_volume
// without the need for re-compilation.
// It must be noted that the files produced with this action become large
// very quickly. Therefore, strict filtering and small number of events are
// encouraged.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------


#ifndef ALL_STEPPING_ACTION_H
#define ALL_STEPPING_ACTION_H

#include <G4UserSteppingAction.hh>
#include <G4ParticleDefinition.hh>
#include <G4GenericMessenger.hh>
#include <G4ThreeVector.hh>
#include <globals.hh>

#include <vector>
#include <map>

class G4Step;

template<typename T>
using StepContainer = std::map<std::pair<G4int, G4String>, std::vector<T>>;


namespace nexus {

  //  Stepping action to analyze the behaviour of optical photons

  class SaveAllSteppingAction: public G4UserSteppingAction
  {
  public:
    /// Constructor
    SaveAllSteppingAction();
    /// Destructor
    ~SaveAllSteppingAction();

    virtual void UserSteppingAction(const G4Step*);

  private:
    G4GenericMessenger* msg_;

    std::vector<G4String>              selected_volumes_;
    std::vector<G4ParticleDefinition*> selected_particles_;

    StepContainer<G4String> initial_volumes_;
    StepContainer<G4String>   final_volumes_;
    StepContainer<G4String>      proc_names_;

    StepContainer<G4ThreeVector> initial_poss_;
    StepContainer<G4ThreeVector>   final_poss_;
    StepContainer<G4double>             times_;

  public:

    StepContainer<G4String> get_initial_volumes();
    StepContainer<G4String> get_final_volumes();
    StepContainer<G4String> get_proc_names();

    StepContainer<G4ThreeVector> get_initial_poss();
    StepContainer<G4ThreeVector> get_final_poss();
    StepContainer<G4double>      get_times();

    void Reset();

  private:
    void   AddSelectedParticle(G4String);
    void   AddSelectedVolume  (G4String);
    G4bool        KeepVolume  (G4String&, G4String&);
    G4bool        KeepParticle(G4ParticleDefinition*);
  };

inline StepContainer<G4String> SaveAllSteppingAction::get_initial_volumes(){return initial_volumes_;}
inline StepContainer<G4String> SaveAllSteppingAction::get_final_volumes  (){return   final_volumes_;}
inline StepContainer<G4String> SaveAllSteppingAction::get_proc_names     (){return      proc_names_;}

inline StepContainer<G4ThreeVector> SaveAllSteppingAction::get_initial_poss(){return initial_poss_;}
inline StepContainer<G4ThreeVector> SaveAllSteppingAction::get_final_poss  (){return   final_poss_;}
inline StepContainer<G4double>      SaveAllSteppingAction::get_times       (){return        times_;}

} // namespace nexus

#endif
