// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 28 July 2009
//
//  Copyright (c) 2009 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "DefaultSteppingAction.h"

#include <G4Step.hh>


namespace nexus {
  
  
  DefaultSteppingAction::DefaultSteppingAction()
  {
  }
  
  
  
  DefaultSteppingAction::~DefaultSteppingAction()
  {
  }
  
  
  
  void DefaultSteppingAction::UserSteppingAction(const G4Step* step)
  { 
  }

  
} // namespace nexus
