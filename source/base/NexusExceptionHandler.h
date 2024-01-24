// ----------------------------------------------------------------------------
// nexus | NexusExceptionHandler.h
//
// This class turns warnings into exceptions, to make the run end.
// It is useful in some tests, or for debugging.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NexusExceptionHandler_h
#define NexusExceptionHandler_h

#include "globals.hh"
#include "G4ExceptionSeverity.hh"
#include "G4VExceptionHandler.hh"

class NexusExceptionHandler : public G4VExceptionHandler
{
  public:

    NexusExceptionHandler();
    virtual ~NexusExceptionHandler();
    G4bool operator==(const NexusExceptionHandler& right) const;
    G4bool operator!=(const NexusExceptionHandler& right) const;

    NexusExceptionHandler(const NexusExceptionHandler&) = delete;
    NexusExceptionHandler& operator=(const NexusExceptionHandler&) = delete;

    virtual G4bool Notify(const char* originOfException,
                          const char* exceptionCode, G4ExceptionSeverity severity,
                          const char* description);
      // Will be invoked by G4StateManager when G4Exception occurs.
      // If TRUE returned, core dump is generated, while if FALSE,
      // the program execution continues.

  private:

    void DumpTrackInfo();
};

#endif
