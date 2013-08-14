//
//  InputManager.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 25 Mar 2009
//     Updated:  6 Apr 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __INPUT_MANAGER__
#define __INPUT_MANAGER__

#include <globals.hh>

namespace bhep{ class gstore; }


namespace nexus {

  class InputManager
  {
  public:
    
    /// Returns the singleton instance
    static InputManager& Instance();
    
    /// 
    void SetInputCard(const G4String&);

    ///
    const bhep::gstore& GetGlobalInput();
    ///
    const bhep::gstore& GetGeometryInput();
    ///
    const bhep::gstore& GetGenerationInput();
    ///
    const bhep::gstore& GetPhysicsInput();

  private:
    
    void ReadInputCard();
    void FillStore(bhep::gstore*, const G4String&);

    private:
    /// Constructor (hidden)
    InputManager();
    /// Destructor (hidden)
    ~InputManager() {}
    /// Copy-constructor (hidden)
    InputManager(InputManager const&);
    /// Assignement operator (hidden)
    InputManager& operator=(InputManager const&);

  private:
    
    bool _init;
    std::string _inputcard;
    
    bhep::gstore* _global;
    bhep::gstore* _geometry;
    bhep::gstore* _generation;
    bhep::gstore* _physics;
  };

} // namespace nexus


#endif
