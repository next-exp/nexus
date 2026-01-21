// ----------------------------------------------------------------------------
// nexus | DefaultSteppingAction.h
//
// This class is the default stepping action of the NEXT simulation.
// It adds the deposited energy to the accumulator in the event action
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef DEFAULTSTEPPINGACTION_H_
#define DEFAULTSTEPPINGACTION_H_

#include <G4UserSteppingAction.hh>

class G4Step;


namespace nexus {

  class DefaultSteppingAction: public G4UserSteppingAction
  {
  public:
    /// Constructor
    DefaultSteppingAction();
    /// Destructor
    ~DefaultSteppingAction();

    void UserSteppingAction(const G4Step*) override;
  };

}

#endif // DEFAULTSTEPPINGACTION_H_
