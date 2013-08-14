
class stepping_verbose;

#ifndef stepping_verbose_h
#define stepping_verbose_h 1

#include "G4SteppingVerbose.hh"

class stepping_verbose : public G4SteppingVerbose 
{
 public:
   
  stepping_verbose();
 ~stepping_verbose();

 public:
  //! implement interface
  void StepInfo();
  void TrackingStarted();

};

#endif
