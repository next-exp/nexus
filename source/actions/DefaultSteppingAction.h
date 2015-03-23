// ----------------------------------------------------------------------------
///  \file   DefaultSteppingAction.h
///  \brief  General-purpose user stepping action.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     24 August 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef DEFAULT_STEPPING_ACTION_H
#define DEFAULT_STEPPING_ACTION_H

#include <G4UserSteppingAction.hh>

class G4Step;


namespace nexus {

  // General-purpose user Stepping action

  class DefaultSteppingAction: public G4UserSteppingAction
  {
  public:
    /// Constructor
    DefaultSteppingAction();
    /// Destructor
    ~DefaultSteppingAction();

    virtual void UserSteppingAction(const G4Step*);
  };

} // namespace nexus

#endif
