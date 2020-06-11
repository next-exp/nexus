// ----------------------------------------------------------------------------
// nexus | ActionFactory.cc
//
// This class instantiates the run, event, tracking, stepping and stacking
// actions that the user specifies via configuration parameters.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ActionsFactory.h"

#include <G4GenericMessenger.hh>
#include <G4UserRunAction.hh>
#include <G4UserEventAction.hh>
#include <G4UserTrackingAction.hh>
#include <G4UserSteppingAction.hh>
#include <G4UserStackingAction.hh>

using namespace nexus;


ActionsFactory::ActionsFactory(): msg_(0)
{
  msg_ = new G4GenericMessenger(this, "/Actions/");
  msg_->DeclareProperty("RegisterRunAction",      runact_name_, "");
  msg_->DeclareProperty("RegisterEventAction",    evtact_name_, "");
  msg_->DeclareProperty("RegisterTrackingAction", trkact_name_, "");
  msg_->DeclareProperty("RegisterSteppingAction", stpact_name_, "");
  msg_->DeclareProperty("RegisterStackingAction", stkact_name_, "");
}


ActionsFactory::~ActionsFactory()
{
  delete msg_;
}


//////////////////////////////////////////////////////////////////////
#include "DefaultRunAction.h"


G4UserRunAction* ActionsFactory::CreateRunAction() const
{
  G4UserRunAction* p = 0;

  if (runact_name_ == "DEFAULT") p = new DefaultRunAction();

  return p;
}


//////////////////////////////////////////////////////////////////////
#include "DefaultEventAction.h"
#include "ELSimEventAction.h"
#include "AnalysisEventAction.h"

G4UserEventAction* ActionsFactory::CreateEventAction() const
{
  G4UserEventAction* p = 0;

  if      (evtact_name_ == "DEFAULT") p = new DefaultEventAction();
  else if (evtact_name_ == "EL_SIM") p = new ELSimEventAction();
  else if (evtact_name_ == "ANALYSIS") p = new AnalysisEventAction();
  else {
    G4String err = "Unknown user event action: " + evtact_name_;
    G4Exception("[ActionsFactory]", "CreateEventAction()", JustWarning, err);
  }

  return p;
}


//////////////////////////////////////////////////////////////////////
#include "DefaultTrackingAction.h"
#include "AnalysisTrackingAction.h"
#include "OpticalTrackingAction.h"

G4UserTrackingAction* ActionsFactory::CreateTrackingAction() const
{
  G4UserTrackingAction* p = 0;

  if (trkact_name_ == "DEFAULT") p = new DefaultTrackingAction();
  else if (trkact_name_ == "ANALYSIS") p = new AnalysisTrackingAction();
  else if (trkact_name_ == "OPTICAL") p = new OpticalTrackingAction();
  else {
    G4String err = "Unknown user tracking action: " + trkact_name_;
    G4Exception("[ActionsFactory]", "CreateTrackingAction()",
      JustWarning, err);
  }

  return p;
}


//////////////////////////////////////////////////////////////////////
#include "DefaultSteppingAction.h"
#include "AnalysisSteppingAction.h"


G4UserSteppingAction* ActionsFactory::CreateSteppingAction() const
{
  G4UserSteppingAction* p = 0;

  if (stpact_name_ == "ANALYSIS") p = new AnalysisSteppingAction();

  else {
    G4String err = "Unknown user stepping action: " + stpact_name_;
    G4Exception("[ActionsFactory]", "CreateSteppingAction()",
      JustWarning, err);
  }

  return p;
}


//////////////////////////////////////////////////////////////////////
#include "DefaultStackingAction.h"

G4UserStackingAction* ActionsFactory::CreateStackingAction() const
{
  G4UserStackingAction* p = 0;

  if (stkact_name_ == "DEFAULT") p = new DefaultStackingAction();

  else {
    G4String err = "Unknown user stacking action: " + stkact_name_;
    G4Exception("[ActionsFactory]", "CreateStackingAction()",
      JustWarning, err);
  }

  return p;
}
