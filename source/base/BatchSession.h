// ----------------------------------------------------------------------------
///  \file   BatchSession.h
///  \brief  
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     22 Mar 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __BATCH_SESSION__
#define __BATCH_SESSION__

#include <G4UIsession.hh>
#include <fstream>


namespace nexus {

  /// TODO. CLASS DESCRIPTION

  class BatchSession: public G4UIsession
  {
  public:
    /// Constructor
    BatchSession(G4String filename, G4UIsession* previous_session=0);
    /// Destructor
    ~BatchSession();

    ///
    virtual G4UIsession* SessionStart();
    ///
    virtual void PauseSessionStart(const G4String& Prompt);

    G4UIsession* GetPreviousSession() const;

  private:
    G4String ReadCommand();
    G4int ExecCommand(const G4String& command);

  private:
    G4bool opened_;
    std::ifstream macrostream_;
    G4UIsession* prev_; ///< Pointer to the previous session
  };

  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline G4UIsession* BatchSession::GetPreviousSession() const 
  { return _prev; }

} // end namespace nexus

#endif
