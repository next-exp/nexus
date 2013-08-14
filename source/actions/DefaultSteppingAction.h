// ----------------------------------------------------------------------------
///  \file   DefaultSteppingAction.h
///  \brief  
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     28 July 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#ifndef __DEFAULT_STEPPING_ACTION__
#define __DEFAULT_STEPPING_ACTION__

#include <G4UserSteppingAction.hh>


namespace nexus {

  class DefaultSteppingAction: public G4UserSteppingAction 
  {
  public:
    /// constructor
    DefaultSteppingAction();
    /// destructor
    ~DefaultSteppingAction();

    void UserSteppingAction(const G4Step*);
  };

} // namespace nexus

#endif

