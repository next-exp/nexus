#ifndef BASE_PERSISTENCY_MANAGER_H
#define BASE_PERSISTENCY_MANAGER_H

#include <G4VPersistencyManager.hh>
#include <G4String.hh>

#include <vector>


class PersistencyManagerBase: public G4VPersistencyManager
  {
  public:

    virtual void OpenFile() = 0;
    virtual void CloseFile() = 0;

    G4String init_macro_;
    std::vector<G4String> macros_;
    std::vector<G4String> delayed_macros_;

    inline void SetMacros(G4String init, std::vector<G4String> mcrs, std::vector<G4String> delayed)
    {init_macro_ = init; macros_ = mcrs; delayed_macros_ = delayed;}


  };


#endif
