//
//  InputManager.cc
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 25 Mar 2009
//     Updated:  6 Apr 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#include "InputManager.h"
#include <bhep/gstore.h>
#include <bhep/sreader.h>


namespace nexus {
  
  
  InputManager::InputManager():
    _global(new bhep::gstore()), _geometry(new bhep::gstore()),
    _generation(new bhep::gstore()), _physics(new bhep::gstore()),
    _init(false)
  {
  }
  
  
  
  InputManager& InputManager::Instance()
  {
    static InputManager mgr;
    return mgr;
  }
  
  
  
  void InputManager::SetInputCard(const G4String& inputcard)
  {
    _inputcard = inputcard;
    ReadInputCard();
  }
  
  
  
  void InputManager::ReadInputCard()
  {
    FillStore(_global,     "GLOBAL");
    FillStore(_geometry,   "GEOMETRY");
    FillStore(_generation, "GENERATION");
    FillStore(_physics,    "PHYSICS");
    _init = true;
  }
  
  
  
  void InputManager::FillStore(bhep::gstore* store, const G4String& group)
  {
    bhep::sreader reader(*store);
    reader.file(_inputcard);
    reader.info_level(bhep::NORMAL);
    reader.group(group);
    reader.read();
  }



  const bhep::gstore& InputManager::GetGlobalInput()
  {
    return *_global;
  }
  
  const bhep::gstore& InputManager::GetGeometryInput()
  {
    return *_geometry;
  }

  const bhep::gstore& InputManager::GetGenerationInput()
  {
    return *_generation;
  }

  const bhep::gstore& InputManager::GetPhysicsInput()
  {
    return *_physics;
  }


} // namespace nexus
