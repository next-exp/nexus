// ----------------------------------------------------------------------------
// nexus | BatchSession.h
//
// This class is used to parse the configuration files and execute the commands
// found in them. One of the features of this class, compared to the G4 one is
// that an error is prompted when a parameter is not recognized.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef BATCH_SESSION_H
#define BATCH_SESSION_H

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
  { return prev_; }

} // end namespace nexus

#endif
