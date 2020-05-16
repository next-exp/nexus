// ----------------------------------------------------------------------------
///  \file   GeantinoSteppingAction.h
///  \brief  Stepping action to record geantino crossing boundaries.
///
///  \author   <gonzalo.martinez.lema@weizmann.ac.il>
///  \date     01 May 2020
///  \version  $Id$
///
///  Copyright (c) 2020 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef GEANTINO_STEPPING_ACTION_H
#define GEANTINO_STEPPING_ACTION_H

#include <G4UserSteppingAction.hh>
#include <G4ParticleDefinition.hh>
#include <G4GenericMessenger.hh>
#include <G4ThreeVector.hh>
#include <globals.hh>

#include <vector>
#include <map>

class G4Step;

template<typename T>
using GeantinoContainer = std::map<G4int, std::vector<T>>;


namespace nexus {

  //  Stepping action to analyze the behaviour of optical photons

  class GeantinoSteppingAction: public G4UserSteppingAction
  {
  public:
    /// Constructor
    GeantinoSteppingAction();
    /// Destructor
    ~GeantinoSteppingAction();

    virtual void UserSteppingAction(const G4Step*);

  private:
    G4GenericMessenger* msg_;

    std::vector<G4String>              selected_volumes_;
    std::vector<G4ParticleDefinition*> selected_particles_;

    GeantinoContainer<G4String> initial_volumes_;
    GeantinoContainer<G4String>   final_volumes_;
    GeantinoContainer<G4String>      proc_names_;

    GeantinoContainer<G4ThreeVector> initial_poss_;
    GeantinoContainer<G4ThreeVector>   final_poss_;

  public:

    GeantinoContainer<G4String> get_initial_volumes();
    GeantinoContainer<G4String> get_final_volumes();
    GeantinoContainer<G4String> get_proc_names();

    GeantinoContainer<G4ThreeVector> get_initial_poss();
    GeantinoContainer<G4ThreeVector> get_final_poss();

    void Reset();

  private:
    void   AddSelectedParticle(G4String);
    void   AddSelectedVolume  (G4String);
    G4bool        KeepVolume  (G4String&, G4String&);
    G4bool        KeepParticle(G4ParticleDefinition*);
  };

inline GeantinoContainer<G4String> GeantinoSteppingAction::get_initial_volumes(){return initial_volumes_;}
inline GeantinoContainer<G4String> GeantinoSteppingAction::get_final_volumes  (){return   final_volumes_;}
inline GeantinoContainer<G4String> GeantinoSteppingAction::get_proc_names     (){return      proc_names_;}

inline GeantinoContainer<G4ThreeVector> GeantinoSteppingAction::get_initial_poss(){return initial_poss_;}
inline GeantinoContainer<G4ThreeVector> GeantinoSteppingAction::get_final_poss  (){return   final_poss_;}

} // namespace nexus

#endif
