//
//  DefaultRunAction.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 15 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __DEFAULT_RUN_ACTION__
#define __DEFAULT_RUN_ACTION__

#include <G4UserRunAction.hh>


namespace nexus {

class DefaultRunAction: public G4UserRunAction
{
public:
  DefaultRunAction() {}
  ~DefaultRunAction() {}
  
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

private:
};

} // namespace nexus

#endif
