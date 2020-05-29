// ----------------------------------------------------------------------------
///  \file   AllSteppingAction.h
///  \brief  Stepping action to record every step of a particle.
///
///  \author   <gonzalo.martinez.lema@weizmann.ac.il>
///  \date     01 May 2020
///  \version  $Id$
///
///  Copyright (c) 2020 NEXT Collaboration. All rights reserved.
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

  class AllSteppingAction: public G4UserSteppingAction
  {
  public:
    /// Constructor
    AllSteppingAction();
    /// Destructor
    ~AllSteppingAction();

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

  public:

    StepContainer<G4String> get_initial_volumes();
    StepContainer<G4String> get_final_volumes();
    StepContainer<G4String> get_proc_names();

    StepContainer<G4ThreeVector> get_initial_poss();
    StepContainer<G4ThreeVector> get_final_poss();

    void Reset();

  private:
    void   AddSelectedParticle(G4String);
    void   AddSelectedVolume  (G4String);
    G4bool        KeepVolume  (G4String&, G4String&);
    G4bool        KeepParticle(G4ParticleDefinition*);
  };

inline StepContainer<G4String> AllSteppingAction::get_initial_volumes(){return initial_volumes_;}
inline StepContainer<G4String> AllSteppingAction::get_final_volumes  (){return   final_volumes_;}
inline StepContainer<G4String> AllSteppingAction::get_proc_names     (){return      proc_names_;}

inline StepContainer<G4ThreeVector> AllSteppingAction::get_initial_poss(){return initial_poss_;}
inline StepContainer<G4ThreeVector> AllSteppingAction::get_final_poss  (){return   final_poss_;}

} // namespace nexus

#endif
