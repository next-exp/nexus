// ----------------------------------------------------------------------------
///  \file   DefaultRunAction.cc
///  \brief  General-purpose user run action.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     15 Apr 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#ifndef __DEFAULT_RUN_ACTION__
#define __DEFAULT_RUN_ACTION__

#include <G4UserRunAction.hh>

class G4Run;


namespace nexus {
  
  /// This class is a general-purpose user run action. It opens the output 
  /// dst at the beginning of the run and closes it at the end.

  class DefaultRunAction: public G4UserRunAction
  {
  public:
    /// Constructor
    DefaultRunAction();
    /// Destructor
    ~DefaultRunAction();
    
    /// This method is invoked at the beginning of the run loop
    void BeginOfRunAction(const G4Run*);
    /// This method is invoked at the end of the run loop
    void EndOfRunAction(const G4Run*);
  };
  
  inline DefaultRunAction::DefaultRunAction() {}
  inline DefaultRunAction::~DefaultRunAction() {}

} // namespace nexus

#endif
