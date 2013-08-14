#ifndef stacking_h
#define stacking_h 1

#include "G4UserStackingAction.hh"
#include "G4ThreeVector.hh"
#include "G4Navigator.hh"
#include "globals.hh"

#include <bhep/utilities.h>
#include <bhep/engine.h>

class G4Track;

class stacking : public G4UserStackingAction , public bhep::engine
{

private:
    G4String ini_part_;
    G4Navigator* navigator_;

public:
    stacking(G4String ini_part);
    ~stacking();

public:
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();
};

#endif

