// NEXT simulation: run.hh
//

#ifndef run_h
#define run_h 1

#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4UserRunAction.hh"
#include "globals.hh"

#include <bhep/bhep_svc.h>
#include <bhep/event.h>
#include <bhep/sreader.h>
#include <bhep/gstore.h>
//#include <bhep/utilities.h>
//#include <bhep/error.h>
#include <bhep/reader_txt.h>
#include <bhep/reader_gz.h>
#include <bhep/reader_hdf5.h>

#include <fstream>


using namespace std;


class G4Run;

class run : public G4UserRunAction 
{

public:
  run(G4String dst_fname);
  ~run() {}
  
public:
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

private:
  G4String dst_fname_;
  G4String dst_ext_;  
};
#endif
